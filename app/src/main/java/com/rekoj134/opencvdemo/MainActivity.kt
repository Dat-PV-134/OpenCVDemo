package com.rekoj134.opencvdemo

import ImageClarityEnhancer
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Bundle
import android.view.View
import android.widget.ImageView
import android.widget.ProgressBar
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import com.rekoj134.nativelib.NativeLib
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext


class MainActivity : AppCompatActivity() {
    private lateinit var imageViewInput: ImageView
    private lateinit var imageViewOutput: ImageView
    private lateinit var progressBar: ProgressBar

    private val coroutineScope = CoroutineScope(Dispatchers.Main + SupervisorJob())
    private val imagePicker = registerForActivityResult(ActivityResultContracts.GetContent()) { uri ->
        uri?.let { processImage(it) }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)

        imageViewInput = findViewById(R.id.imageViewDefault)
        imageViewOutput = findViewById(R.id.imageViewSegmented)
        progressBar = findViewById(R.id.progressBar)

        // Click để chọn ảnh
        imageViewInput.setOnClickListener {
            imagePicker.launch("image/*")
        }

        Toast.makeText(this, "Nhấn vào ảnh để chọn từ thư viện", Toast.LENGTH_LONG).show()
    }

    private fun processImage(uri: Uri) {
        progressBar.visibility = View.VISIBLE

        coroutineScope.launch {
            val inputStream = contentResolver.openInputStream(uri)
            val srcBitmap = BitmapFactory.decodeStream(inputStream)

//            val dstBitmap = createBitmap(srcBitmap.width, srcBitmap.height)

            // Gọi hàm native xử lý
//            nativeLib.enhanceImage(srcBitmap, dstBitmap)

            imageViewInput.setImageBitmap(srcBitmap)

            val enhancer = ImageClarityEnhancer(this@MainActivity)
            val outputBitmap = withContext(Dispatchers.IO) {
                enhancer.enhanceClarity(srcBitmap)
            }

            imageViewOutput.setImageBitmap(outputBitmap)
            progressBar.visibility = View.GONE
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        coroutineScope.cancel()
    }
}


