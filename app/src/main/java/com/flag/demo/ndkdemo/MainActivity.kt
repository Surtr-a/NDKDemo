package com.flag.demo.ndkdemo

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.recyclerview.widget.LinearLayoutManager
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        initView()
    }

    private fun initView() {
        val btnNameList = arrayListOf<String>()
        btnNameList.add("BitmapDemo")
        btnNameList.add("FaceDetectionDemo")
        btnNameList.add("AudioPlayerDemo")

        val adapter = MainAdapter(btnNameList, this)
        rv_main.layoutManager = LinearLayoutManager(this)
        rv_main.adapter = adapter
        adapter.onButtonClickListener {
            when(it) {
                0 -> {
                    startActivity(Intent(this, BitmapDemoActivity::class.java))
                }
                1 -> {
                    startActivity(Intent(this, FaceDetectionActivity::class.java))
                }
                2 -> {
                    startActivity(Intent(this, AudioPlayerDemoActivity::class.java))
                }
            }
        }
    }
}
