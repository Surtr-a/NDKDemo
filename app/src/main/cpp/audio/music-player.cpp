//
// Created by 30399 on 2020/7/29.
//

#include <jni.h>
#include <malloc.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// 缩放
#include "libswscale/swscale.h"
// 重采样
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
}

#include "../android_log.h"
#include "MyJNICall.h"
#include "MyFFmpeg.h"

MyJNICall *myJNICall;
MyFFmpeg *myFFmpeg;

// 子线程不共享 JNIEnv
JavaVM *pJavaVm = nullptr;
// 重写 so 被加载时被调用的一个方法
extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *javaVm, void *reserved) {
    pJavaVm = javaVm;
    JNIEnv *myEnv;
    if (javaVm->GetEnv(reinterpret_cast<void **>(&myEnv), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_MyPlayer_nPrepare(JNIEnv *env, jobject thiz, jstring url) {
    char *url_ = const_cast<char *>(env->GetStringUTFChars(url, nullptr));
    if (myFFmpeg == nullptr) {
        myJNICall = new MyJNICall(env, pJavaVm, thiz);
        myFFmpeg = new MyFFmpeg(myJNICall, url_);
        myFFmpeg->prepare();
    }
    env->ReleaseStringUTFChars(url, url_);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_MyPlayer_nPrepareAsync(JNIEnv *env, jobject thiz, jstring url) {
    char *url_ = const_cast<char *>(env->GetStringUTFChars(url, nullptr));
    if (myFFmpeg == nullptr) {
        myJNICall = new MyJNICall(env, pJavaVm, thiz);
        myFFmpeg = new MyFFmpeg(myJNICall, url_);
        myFFmpeg->prepareAsync();
    }
    env->ReleaseStringUTFChars(url, url_);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_MyPlayer_nPlay(JNIEnv *env, jobject thiz, jstring url) {
    if (myFFmpeg != nullptr) {
        myFFmpeg->play();
    }
}