import android.content.Context
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import com.rekoj134.nativelib.NativeLib
import org.tensorflow.lite.Interpreter
import java.io.FileInputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.channels.FileChannel

class ImageClarityEnhancer(context: Context) {

    private val interpreter: Interpreter
    private val nativeLib = NativeLib()

    init {
        val modelBuffer = loadModelFile(context, "Real-ESRGAN-General-x4v3.tflite")
        val options = Interpreter.Options()
        interpreter = Interpreter(modelBuffer, options)
    }

    private fun loadModelFile(context: Context, filename: String): ByteBuffer {
        val fileDescriptor = context.assets.openFd(filename)
        val inputStream = FileInputStream(fileDescriptor.fileDescriptor)
        val fileChannel = inputStream.channel
        val startOffset = fileDescriptor.startOffset
        val declaredLength = fileDescriptor.declaredLength
        return fileChannel.map(FileChannel.MapMode.READ_ONLY, startOffset, declaredLength)
    }

    fun enhanceClarity(bitmap: Bitmap): Bitmap {
        val inputSize = 128  // Hoặc 256 tùy mô hình yêu cầu

        // Kiểm tra tỷ lệ khung hình của ảnh đầu vào
        val aspectRatio = bitmap.width.toFloat() / bitmap.height.toFloat()

        // Tính toán chiều rộng và chiều cao mới để giữ tỷ lệ khung hình
        var resizedWidth = inputSize
        var resizedHeight = (inputSize / aspectRatio).toInt()

        // Nếu chiều cao tính toán vượt quá kích thước input, resize theo chiều cao thay vì chiều rộng
        if (resizedHeight > inputSize) {
            resizedHeight = inputSize
            resizedWidth = (inputSize * aspectRatio).toInt()
        }

        // Tạo Bitmap mới cho resized
        val resizedBitmap = Bitmap.createBitmap(resizedWidth, resizedHeight, Bitmap.Config.ARGB_8888)

        // Gọi hàm resize C++ qua JNI để resize ảnh
        nativeLib.resizeImage(bitmap, resizedBitmap, resizedWidth, resizedHeight)

        // Tạo một Bitmap mới để chứa ảnh resized với kích thước chính thức inputSize
        val finalBitmap = Bitmap.createBitmap(inputSize, inputSize, Bitmap.Config.ARGB_8888)
        val canvas = Canvas(finalBitmap)

        // Tính toán để căn giữa ảnh resized trong finalBitmap (thêm padding)
        val left = (inputSize - resizedWidth) / 2
        val top = (inputSize - resizedHeight) / 2

        // Vẽ ảnh resized vào canvas, căn giữa ảnh trong finalBitmap
        canvas.drawBitmap(resizedBitmap, left.toFloat(), top.toFloat(), null)

        // Chuẩn bị input: (1, H, W, 3), float32
        val inputBuffer = ByteBuffer.allocateDirect(1 * inputSize * inputSize * 3 * 4)
        inputBuffer.order(ByteOrder.nativeOrder())
        for (y in 0 until inputSize) {
            for (x in 0 until inputSize) {
                val pixel = finalBitmap.getPixel(x, y)
                inputBuffer.putFloat((pixel shr 16 and 0xFF) / 255f)  // R
                inputBuffer.putFloat((pixel shr 8 and 0xFF) / 255f)   // G
                inputBuffer.putFloat((pixel and 0xFF) / 255f)         // B
            }
        }

        // Output: (1, inputSize*4, inputSize*4, 3)
        val outputSize = inputSize * 4
        val output = Array(1) { Array(outputSize) { Array(outputSize) { FloatArray(3) } } }

        // Inference
        interpreter.run(inputBuffer, output)

        // Chuyển output về Bitmap
        val outputBitmap = Bitmap.createBitmap(outputSize, outputSize, Bitmap.Config.ARGB_8888)
        for (y in 0 until outputSize) {
            for (x in 0 until outputSize) {
                val r = (output[0][y][x][0] * 255).toInt().coerceIn(0, 255)
                val g = (output[0][y][x][1] * 255).toInt().coerceIn(0, 255)
                val b = (output[0][y][x][2] * 255).toInt().coerceIn(0, 255)
                outputBitmap.setPixel(x, y, Color.rgb(r, g, b))
            }
        }

        // Tính tỷ lệ khung hình của ảnh output và ảnh gốc
        val ratioWidth = bitmap.width.toFloat() / outputBitmap.width.toFloat()
        val ratioHeight = bitmap.height.toFloat() / outputBitmap.height.toFloat()

        // Chọn tỷ lệ nhỏ nhất để giữ nguyên tỷ lệ khung hình
        val ratio = Math.min(ratioWidth, ratioHeight)

        // Resize ảnh output về đúng tỷ lệ
        return Bitmap.createScaledBitmap(
            outputBitmap,
            (outputBitmap.width * ratio).toInt(),
            (outputBitmap.height * ratio).toInt(),
            true
        )
    }
}
