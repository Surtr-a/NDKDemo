package com.flag.demo.ndkdemo

import android.graphics.*

class BitmapUtils {
    companion object {
        fun gray(src: Bitmap): Bitmap {
            val dst = Bitmap.createBitmap(src.width, src.height, src.config)
            val canvas = Canvas(dst)

            val paint = Paint()
            paint.isDither = true   // 设置防抖动（过渡更加自然）
            paint.isAntiAlias = true    // 消除锯齿

//            val colorMatrix = ColorMatrix() // 可以通过构造器自己传入矩阵（5*4）
//            colorMatrix.setSaturation(0f)   // 设置饱和度，底层对矩阵进行操作
            // 直接传入矩阵
            val colorMatrix = ColorMatrix(
                floatArrayOf(
                    0.213f, 0.715f, 0.072f, 0f, 0f,
                    0.213f, 0.715f, 0.072f, 0f, 0f,
                    0.213f, 0.715f, 0.072f, 0f, 0f,
                    0f, 0f, 0f, 1f, 0f
                )
            )

            paint.colorFilter = ColorMatrixColorFilter(colorMatrix)

            canvas.drawBitmap(src, 0f, 0f, paint)

            return dst
        }

        fun gray2(src: Bitmap): Bitmap {
            val dst = Bitmap.createBitmap(src.width, src.height, src.config)
            val pixels = IntArray(src.width * src.height)
            src.getPixels(pixels, 0, src.width, 0, 0, src.width, src.height)

            // 遍历像素点，每个像素点包含ARGB四个颜色通道信息，每个通道信息占8位
            for ((index, it) in pixels.withIndex()) {
                val a = it.shr(24) and 0xff // 右移24位并和 1111 1111 进行与操作
                val r = it.shr(16) and 0xff
                val g = it.shr(8) and 0xff
                val b = it and 0xff

                val gray = (0.213 * r + 0.715 * g + 0.072 * b).toInt()
                pixels[index] = (a.shl(24)) or (gray.shl(16)) or (gray.shl(8)) or gray
            }

            dst.setPixels(pixels, 0, src.width, 0, 0, src.width, src.height)
            return dst
        }

        external fun gray3(src: Bitmap): Int    // 返回是否成功
        external fun imageChangeFromArr(src: Bitmap, rows: Int, cols: Int): Int
        // 获取直方图
        external fun getImageHistogram(
            src: Bitmap,
            rows: Int,
            cols: Int,
            dst: Bitmap?,
            type: Bitmap.Config
        ): Int
        // 逆世界
        external fun againstWorld(src: Bitmap): Int
        // 浮雕
        external fun relief(src: Bitmap): Int
        // 马赛克
        external fun mosaic(src: Bitmap): Int
        // 毛玻璃
        external fun groundGlass(src: Bitmap): Int
        // 油画
        external fun oilPainting(src: Bitmap): Int
        // 旋转
        external fun rotation(src: Bitmap, dst: Bitmap): Int
        // 仿射变换
        external fun wrapAffine(src: Bitmap): Int
        // 缩放
        external fun reSize(src: Bitmap, dst: Bitmap, width: Int, height: Int): Int
        // 重映射
        external fun reMap(src: Bitmap): Int
        // 灰度图
        external fun gray4(src: Bitmap): Int
        // 直方均衡
        external fun equalizeHist(src: Bitmap): Int
        // 积分图均值模糊
        external fun meanBlurByInteg(src: Bitmap): Int
        // 二维码区域提取
        external fun getQRCodeArea(src: Bitmap): Int
        // HOG特征匹配
        external fun hogBitmap(src: Bitmap): Int
        // LBP特征匹配
        external fun lbpBitmap(src: Bitmap): Int
    }
}
