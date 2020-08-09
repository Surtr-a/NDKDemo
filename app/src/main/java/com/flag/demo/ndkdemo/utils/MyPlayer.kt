package com.flag.demo.ndkdemo.utils

import android.text.TextUtils
import com.flag.demo.ndkdemo.MediaErrorListener
import com.flag.demo.ndkdemo.MediaPreparedListener
import java.lang.NullPointerException

// 音频播放处理类
class MyPlayer {
    init {
        System.loadLibrary("native-lib")
    }

    // url 可以是本地文件路径，也可以是 http 链接
    private lateinit var url: String

    private lateinit var mediaErrorListener: MediaErrorListener
    private lateinit var mediaPreparedListener: MediaPreparedListener

    fun setOnErrorListener(mediaErrorListener: MediaErrorListener) {
        this.mediaErrorListener = mediaErrorListener
    }
    fun setOnPreparedListener(mediaPreparedListener: MediaPreparedListener) {
        this.mediaPreparedListener = mediaPreparedListener
    }

    // called by jni
    private fun onError(code: Int, msg: String) {
        if (mediaErrorListener != null) {
            mediaErrorListener.onError(code, msg)
        }
    }
    private fun onPrepared() {
        if (mediaPreparedListener != null) {
            mediaPreparedListener.onPrepared()
        }
    }

    fun setDataSource(url: String) {
        this.url = url
    }

    fun play() {
        if (TextUtils.isEmpty(url)) {
            throw NullPointerException("url is empty, please call method setDataSource")
        }
        nPlay(url)
    }

    fun prepare() {
        if (TextUtils.isEmpty(url)) {
            throw NullPointerException("url is empty, please call method setDataSource")
        }
        nPrepare(url)
    }

    fun prepareAsync() {
        if (TextUtils.isEmpty(url)) {
            throw NullPointerException("url is empty, please call method setDataSource")
        }
        nPrepareAsync(url)
    }

    private external fun nPlay(url: String)
    private external fun nPrepare(url: String)
    private external fun nPrepareAsync(url: String)
}