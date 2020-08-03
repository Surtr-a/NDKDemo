package com.flag.demo.ndkdemo.opencv

import java.lang.UnsupportedOperationException

class Mat {
    private var rows = 0
    private var cols = 0
    private var type: CVType? = null
    val mNativePtr: Long

    constructor(rows: Int, cols: Int, type: CVType) {
        this.rows = rows
        this.cols = cols
        this.type = type
        mNativePtr = newMatIII(rows, cols, type.value)
    }

    constructor() {
        mNativePtr = newMat()
    }

    fun put(row: Int, col: Int, value: Float) {
        if (type != CVType.CV_32FC1) {
            throw UnsupportedOperationException("Unsupported Data Type")
        }
        nPutF(mNativePtr, row, col, value)
    }

    private external fun newMat(): Long
    private external fun newMatIII(rows: Int, cols: Int, value: Int): Long
    private external fun nPutF(nativePtr: Long, row: Int, col: Int, value: Float)
    // 释放Mat
    private external fun nDelete(mNativePtr: Long)

    // 在Kotlin中直接声明finalize函数
    protected fun finalize() {
        nDelete(mNativePtr)
    }
}