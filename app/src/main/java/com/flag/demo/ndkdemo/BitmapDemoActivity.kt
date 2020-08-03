package com.flag.demo.ndkdemo

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.flag.demo.ndkdemo.utils.OpenCVUtil
import kotlinx.android.synthetic.main.activity_bitmap_demo.*

class BitmapDemoActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_bitmap_demo)

        val options = BitmapFactory.Options()
        options.inMutable = true
//        options.inPreferredConfig = Bitmap.Config.RGB_565
        val src = BitmapFactory.decodeResource(resources, R.mipmap.test3, options)

        // 在native层处理图片
//        val rows = src.height
//        val cols = src.width
//        BitmapUtils.imageChangeFromArr(src, rows, cols)
//        BitmapUtils.againstWorld(src)
//        BitmapUtils.mosaic(src)
//        BitmapUtils.relief(src)
//        BitmapUtils.groundGlass(src)
//        BitmapUtils.oilPainting(src)
//        val newBitmap = Bitmap.createBitmap(src.height, src.width, src.config)
//        BitmapUtils.rotation(src, newBitmap)
//        BitmapUtils.wrapAffine(src)
//        val newBitmap = Bitmap.createBitmap(src.width / 2, src.height / 2, src.config)
//        BitmapUtils.reSize(src, newBitmap, src.width / 2, src.height / 2)
//        BitmapUtils.reMap(src)
//        BitmapUtils.gray4(src)
//        image_1.setImageBitmap(src)
//        val newBitmap = Bitmap.createBitmap(src)
//        BitmapUtils.meanBlurByInteg(src)
//        BitmapUtils.getQRCodeArea(src)
//        BitmapUtils.hogBitmap(src);
        BitmapUtils.lbpBitmap(src)
        image_2.setImageBitmap(src)

        // 获取直方图
//        val dst: Bitmap? = null
//        BitmapUtils.getImageHistogram(src, rows, cols, dst, Bitmap.Config.ARGB_8888)
//        image_2.setImageBitmap(dst)
    }

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}