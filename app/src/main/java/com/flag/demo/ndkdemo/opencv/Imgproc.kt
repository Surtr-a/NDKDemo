package com.flag.demo.ndkdemo.opencv

class Imgproc {
    companion object {
        fun filter2D(src: Mat, dst: Mat, kernel: Mat) {
            nFilter2D(src.mNativePtr, dst.mNativePtr, kernel.mNativePtr)
        }

        private external fun nFilter2D(srcPtr: Long, dstPtr: Long, kernelPtr: Long)
    }
}