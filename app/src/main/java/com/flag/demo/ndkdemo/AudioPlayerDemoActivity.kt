package com.flag.demo.ndkdemo

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import com.flag.demo.ndkdemo.utils.MyPlayer
import java.io.File

class AudioPlayerDemoActivity : AppCompatActivity() {

    val mMusicFile = File(Environment.getExternalStorageDirectory(), "netease/cloudmusic/Music/コミネリサ - Resuscitated Hope.flac")
    private lateinit var myPlayer: MyPlayer

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_audio_player_demo)

        myPlayer = MyPlayer()
        myPlayer.setDataSource(mMusicFile.absolutePath)
        myPlayer.setOnErrorListener(MediaErrorListener { code, msg ->
            Log.e("TAG", "error code: $code")
            Log.e("TAG", "msg: $msg")
        })
        myPlayer.setOnPreparedListener(MediaPreparedListener {
            // 准备完毕再播放
            myPlayer.play()
        })
        myPlayer.prepareAsync()
    }
}