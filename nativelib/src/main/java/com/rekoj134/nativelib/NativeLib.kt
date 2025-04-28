package com.rekoj134.nativelib

import android.graphics.Bitmap

class NativeLib {

    external fun stringFromJNI(): String

    external fun gray(src: Bitmap, dst: Bitmap)

    companion object {
        // Used to load the 'nativelib' library on application startup.
        init {
            System.loadLibrary("nativelib")
        }
    }
}