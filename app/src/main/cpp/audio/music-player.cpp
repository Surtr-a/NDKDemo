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

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_MyPlayer_nPlay(JNIEnv *env, jobject thiz, jstring url) {
    const char *url_ = env->GetStringUTFChars(url, nullptr);
    myJNICall = new MyJNICall(env, nullptr);
    myFFmpeg = new MyFFmpeg(myJNICall, url_);
    myFFmpeg->play();

    delete myFFmpeg;
    delete myJNICall;
    env->ReleaseStringUTFChars(url, url_);
}