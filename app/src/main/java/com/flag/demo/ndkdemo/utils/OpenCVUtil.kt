package com.flag.demo.ndkdemo.utils

import android.graphics.Bitmap
import android.util.Log
import android.util.Size
import com.flag.demo.ndkdemo.opencv.BMUtil
import com.flag.demo.ndkdemo.opencv.CVType
import com.flag.demo.ndkdemo.opencv.Imgproc
import com.flag.demo.ndkdemo.opencv.Mat

class OpenCVUtil {
    companion object {
        fun blur(bitmap: Bitmap): Bitmap {
            val srcMat = Mat();
            BMUtil.bitmap2mat(bitmap, srcMat)

            val size = 15;
            val kernel = Mat(size, size, CVType.CV_32FC1)
            for (i in 0 until size) {
                for (j in 0 until size) {
                    kernel.put(i, j, 1f / (size * size))
                }
            }

            val dst = Mat()
            Imgproc.filter2D(srcMat, dst, kernel)

            BMUtil.mat2bitmap(dst, bitmap)
            return bitmap
        }
    }
}