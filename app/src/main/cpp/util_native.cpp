//
// Created by 30399 on 2020/7/3.
//

#include <jni.h>
#include "opencv2/opencv.hpp"
#include "cv_helper.h"

using namespace cv;

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_opencv_BMUtil_00024Companion_bitmap2mat(JNIEnv *env, jobject thiz,
        jobject bitmap, jlong mat_ptr) {
    Mat *mat = reinterpret_cast<Mat *>(mat_ptr);
    cv_helper::bitmap2mat(env, bitmap, *mat);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_opencv_BMUtil_00024Companion_mat2bitmap(JNIEnv *env, jobject thiz,
        jlong mat_ptr, jobject bitmap) {
    Mat *mat = reinterpret_cast<Mat *>(mat_ptr);
    cv_helper::mat2bitmap(env, *mat, bitmap);
}