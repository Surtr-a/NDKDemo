package com.flag.demo.ndkdemo.opencv

import android.graphics.Bitmap

class BMUtil {
    companion object {
        fun bitmap2mat(bitmap: Bitmap, mat: Mat) {
            bitmap2mat(bitmap, mat.mNativePtr)
        }

        fun mat2bitmap(mat: Mat, bitmap: Bitmap) {
            mat2bitmap(mat.mNativePtr, bitmap)
        }

        private external fun bitmap2mat(bitmap: Bitmap, matPtr: Long)
        private external fun mat2bitmap(matPtr: Long, bitmap: Bitmap)
    }
}