package com.rekoj134.nativelib

import android.graphics.Bitmap

class NativeLib {

    external fun stringFromJNI(): String

    external fun gray(src: Bitmap, dst: Bitmap)

    external fun test(src: Bitmap, dst: Bitmap)

    external fun enhanceImage(src: Bitmap, dst: Bitmap)

    external fun resizeImage(src: Bitmap, dst: Bitmap, targetWidth: Int, targetHeight: Int)

    companion object {
        // Used to load the 'nativelib' library on application startup.
        init {
            System.loadLibrary("nativelib")
        }
    }
}