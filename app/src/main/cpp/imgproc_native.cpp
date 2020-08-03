//
// Created by 30399 on 2020/7/3.
//

#include <jni.h>
#include "opencv2/opencv.hpp"

using namespace cv;

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_opencv_Imgproc_00024Companion_nFilter2D(JNIEnv *env, jobject thiz,
        jlong src_ptr, jlong dst_ptr,
        jlong kernel_ptr) {
    Mat *src = reinterpret_cast<Mat *>(src_ptr);
    Mat *dst = reinterpret_cast<Mat *>(dst_ptr);
    Mat *kernel = reinterpret_cast<Mat *>(kernel_ptr);

    filter2D(*src, *dst, src->depth(), *kernel);
}