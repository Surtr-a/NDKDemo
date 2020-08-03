package com.flag.demo.ndkdemo.utils

import org.opencv.core.Mat

class FaceDetection {
    init {
        System.loadLibrary("native-lib")
    }

    fun faceDetection(mat: Mat) {
        faceDetection(mat.nativeObj)
    }

    // 加载级联分类器
    external fun loadCascade(filePath: String)

    private external fun faceDetection(nativeObj: Long)
    // 训练样本
    external fun trainingPattern()
    // 加载样本数据
    external fun loadPattern(patternPath: String)
}