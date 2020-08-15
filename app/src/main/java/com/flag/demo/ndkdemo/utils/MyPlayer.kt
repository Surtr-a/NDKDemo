package com.flag.demo.ndkdemo.utils

import android.text.TextUtils
import android.view.Surface
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

    // 错误监听和播放准备完成监听
    private lateinit var mediaErrorListener: MediaErrorListener
    private lateinit var mediaPreparedListener: MediaPreparedListener

    fun setOnErrorListener(mediaErrorListener: MediaErrorListener) {
        this.mediaErrorListener = mediaErrorListener
    }
    fun setOnPreparedListener(mediaPreparedListener: MediaPreparedListener) {
        this.mediaPreparedListener = mediaPreparedListener
    }

    // called by jni
    fun onError(code: Int, msg: String) {
        if (mediaErrorListener != null) {
            mediaErrorListener.onError(code, msg)
        }
    }
    fun onPrepared() {
        if (mediaPreparedListener != null) {
            mediaPreparedListener.onPrepared()
        }
    }

    // 设置 url
    fun setDataSource(url: String) {
        this.url = url
    }

    // 播放
    fun play() {
        if (TextUtils.isEmpty(url)) {
            throw NullPointerException("url is empty, please call method setDataSource")
        }
        nPlay(url)
    }

    // 同步准备
    fun prepare() {
        if (TextUtils.isEmpty(url)) {
            throw NullPointerException("url is empty, please call method setDataSource")
        }
        nPrepare(url)
    }

    // 异步准备
    fun prepareAsync() {
        if (TextUtils.isEmpty(url)) {
            throw NullPointerException("url is empty, please call method setDataSource")
        }
        nPrepareAsync(url)
    }

    fun stop() {

    }

    fun setSurface(surface: Surface) {
        nSetSurface(surface)
    }

    private external fun nPlay(url: String)
    private external fun nPrepare(url: String)
    private external fun nPrepareAsync(url: String)
    private external fun nSetSurface(surface: Surface)
}