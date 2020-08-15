package com.flag.demo.ndkdemo

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.view.View
import kotlinx.android.synthetic.main.activity_video_player_demo.*
import java.io.File

class VideoPlayerDemoActivity : AppCompatActivity() {
    private val videoFile = File(Environment.getExternalStorageDirectory(), "Pictures/QQ/李荣浩 - 年少有为.mp4")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_video_player_demo)
    }

    fun play(view: View) {
        video_view.play(videoFile.absolutePath)
    }
}