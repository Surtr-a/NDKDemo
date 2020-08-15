package com.flag.demo.ndkdemo

import android.content.Context
import android.graphics.PixelFormat
import android.util.AttributeSet
import android.util.Log
import android.view.SurfaceView
import com.flag.demo.ndkdemo.utils.MyPlayer

class MyVideoView : SurfaceView, MediaPreparedListener, MediaErrorListener {
    private lateinit var myPlayer: MyPlayer

    constructor(context: Context) : this(context, null)
    constructor(context: Context, attrs: AttributeSet?) : this(context, attrs, 0)
    constructor(context: Context, attrs: AttributeSet?, defStyleAttr: Int) : super(context, attrs, defStyleAttr) {
        val holder = holder;
        holder.setFormat(PixelFormat.RGBA_8888)
        myPlayer = MyPlayer()
        myPlayer.setOnPreparedListener(this)
        myPlayer.setOnErrorListener(this)
    }

    fun play(url: String) {
        myPlayer.setDataSource(url)
        myPlayer.prepareAsync()
    }

    fun stop() {
        myPlayer.stop()
    }

    override fun onPrepared() {
        myPlayer.setSurface(holder.surface)
        myPlayer.play()
    }

    override fun onError(code: Int, msg: String?) {
        Log.e("TTTTTTT", "onError: ")
    }

}