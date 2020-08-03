//
// Created by 30399 on 2020/7/3.
//
#include <jni.h>
#include "opencv2/opencv.hpp"

using namespace cv;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_flag_demo_ndkdemo_opencv_Mat_newMat(JNIEnv *env, jobject thiz) {
    Mat *mat = new Mat();
    return reinterpret_cast<jlong>(mat);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_flag_demo_ndkdemo_opencv_Mat_newMatIII(JNIEnv *env, jobject thiz, jint rows, jint cols,
                                                jint type) {
    Mat *mat = new Mat(rows, cols, type);
    return reinterpret_cast<jlong>(mat);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_opencv_Mat_nPutF(JNIEnv *env, jobject thiz, jlong native_ptr, jint row,
                                            jint col, jfloat value) {
    Mat *mat = reinterpret_cast<Mat *>(native_ptr);
    mat->at<float>(row, col) = value;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_opencv_Mat_nDelete(JNIEnv *env, jobject thiz, jlong m_native_ptr) {
    Mat *mat = reinterpret_cast<Mat *>(m_native_ptr);
    delete(mat);
}