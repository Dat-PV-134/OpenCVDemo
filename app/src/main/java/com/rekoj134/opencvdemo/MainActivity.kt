package com.rekoj134.opencvdemo

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.rekoj134.nativelib.NativeLib
import com.rekoj134.opencvdemo.databinding.ActivityMainBinding
import androidx.core.graphics.createBitmap

class MainActivity : AppCompatActivity() {
    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        val nativeLibInstance = NativeLib()
        binding.tvHello.text = nativeLibInstance.stringFromJNI()

        // Load ảnh từ drawable
        val srcBitmap = BitmapFactory.decodeResource(resources, R.drawable.img_test)

        // Tạo Bitmap output
        val grayBitmap = createBitmap(srcBitmap.width, srcBitmap.height)

        // Convert ảnh sang grayscale
//        nativeLibInstance.gray(srcBitmap, grayBitmap)
        nativeLibInstance.test(srcBitmap, grayBitmap)

        // Set ảnh grayscale lên ImageView
        binding.imgTest.setImageBitmap(grayBitmap)
    }
}