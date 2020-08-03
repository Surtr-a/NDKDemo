package com.flag.demo.ndkdemo

import android.Manifest
import android.content.Context
import android.content.pm.ActivityInfo
import android.content.pm.PackageManager
import android.hardware.Camera
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.annotation.RequiresApi
import androidx.core.app.ActivityCompat
import com.flag.demo.ndkdemo.utils.FaceDetection
import kotlinx.android.synthetic.main.activity_face_detection.*
import org.jetbrains.anko.alert
import org.jetbrains.anko.noButton
import org.jetbrains.anko.toast
import org.jetbrains.anko.yesButton
import org.opencv.android.CameraBridgeViewBase
import org.opencv.core.Core
import org.opencv.core.Mat
import java.io.File
import java.io.FileOutputStream
import java.io.IOException

class FaceDetectionActivity : AppCompatActivity(), CameraBridgeViewBase.CvCameraViewListener {

    init {
        System.loadLibrary("native-lib")
        System.loadLibrary("opencv_java4")
    }

    private lateinit var mFaceDetection: FaceDetection
    private lateinit var mCascadeFile: File

    @RequiresApi(Build.VERSION_CODES.M)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_face_detection)

        requestPermission {
            // 打开前置摄像头
            simple_camera.cameraIndex = Camera.CameraInfo.CAMERA_FACING_BACK
            simple_camera.setCvCameraViewListener(this)
        }

        copyCascadeFile()
        mFaceDetection = FaceDetection()
        mFaceDetection.loadCascade(mCascadeFile.absolutePath)

        // 人脸样本采集
        // mFaceDetection.trainingPattern()

        // 加载采集好的样本
        mFaceDetection.loadPattern("/storage/emulated/0/facedetection/face_my_pattern.xml")
    }

    override fun onResume() {
        super.onResume()
        simple_camera.enableView()
    }

    override fun onPause() {
        super.onPause()
        simple_camera.disableView()
    }

    private fun copyCascadeFile() {
        try {
            // 加载级联分类器
            val inputStream = resources.openRawResource(R.raw.haarcascade_frontalface_default)
            val cascadeDir = getDir("cascade", Context.MODE_PRIVATE)
            mCascadeFile = File(cascadeDir, "haarcascade_frontalface_default.xml")
            if (mCascadeFile.exists()) return
            val outputStream = FileOutputStream(mCascadeFile)

            val buffer = ByteArray(4096)
            var bytesRead: Int
            while (true) {
                bytesRead = inputStream.read(buffer)
                if (bytesRead != -1) {
                    outputStream.write(buffer, 0, bytesRead)
                } else {
                    break
                }
            }
            inputStream.close()
            outputStream.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    override fun onCameraViewStarted(width: Int, height: Int) {

    }

    override fun onCameraViewStopped() {

    }

    // 相机数据 -> Mat -> SurfaceView
    override fun onCameraFrame(inputFrame: Mat?): Mat? {
        // 业务逻辑
        // 默认横屏，竖屏状态需要旋转
        if (this.resources.configuration.orientation
            == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            && simple_camera.cameraIndex == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            Core.rotate(inputFrame, inputFrame, Core.ROTATE_90_COUNTERCLOCKWISE)
        } else if (this.resources.configuration.orientation
            == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
            && simple_camera.cameraIndex == Camera.CameraInfo.CAMERA_FACING_BACK) {
            Core.rotate(inputFrame, inputFrame, Core.ROTATE_90_CLOCKWISE)
        }
        // 传递每一帧
        inputFrame?.let { mFaceDetection.faceDetection(it) }
        return inputFrame
    }

    @RequiresApi(Build.VERSION_CODES.M)
    private fun requestPermission(onSuccess: () -> Unit) {
        val permission = Manifest.permission.WRITE_EXTERNAL_STORAGE
        val checkSelfPermission = ActivityCompat.checkSelfPermission(this, permission)
        if (checkSelfPermission == PackageManager.PERMISSION_GRANTED) {
            onSuccess()
        } else {
            if (ActivityCompat.shouldShowRequestPermissionRationale(this, permission)) {
                alert("取消授权将使某些功能受限", "温馨提示") {
                    yesButton { myRequestPermission() }
                    noButton { finish() }
                }.show()
            } else {
                myRequestPermission()
            }
        }
    }

    @RequiresApi(Build.VERSION_CODES.M)
    private fun myRequestPermission() {
        val permissions = arrayOf(Manifest.permission.CAMERA, Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE)
        requestPermissions(permissions, 1)
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
            toast("授权成功！")
        }
    }
}