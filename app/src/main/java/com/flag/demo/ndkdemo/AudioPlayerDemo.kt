package com.flag.demo.ndkdemo

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import com.flag.demo.ndkdemo.utils.MyPlayer
import java.io.File

class AudioPlayerDemo : AppCompatActivity() {

    val mMusicFile = File(Environment.getExternalStorageDirectory(), "netease/cloudmusic/Music/コミネリサ - Resuscitated Hope.flac")
    private lateinit var myPlayer: MyPlayer

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_audio_player_demo)

        myPlayer = MyPlayer()
        myPlayer.setDataSource(mMusicFile.absolutePath)
        myPlayer.play()
    }
}