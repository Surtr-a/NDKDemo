package com.flag.demo.ndkdemo.utils

import android.text.TextUtils
import java.lang.NullPointerException

// 音频播放处理类
class MyPlayer {
    init {
        System.loadLibrary("native-lib")
    }

    // url 可以是本地文件路径，也可以是 http 链接
    private lateinit var url: String

    fun setDataSource(url: String) {
        this.url = url
    }

    fun play() {
        if (TextUtils.isEmpty(url)) {
            throw NullPointerException("url is empty, please call method setDataSource")
        }
        nPlay(url)
    }

    external fun nPlay(url: String)
}