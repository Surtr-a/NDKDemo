//
// Created by jiuxs on 20-8-4.
//

#ifndef NDKDEMO_MYJNICALL_H
#define NDKDEMO_MYJNICALL_H


#include "jni.h"
#include "../android_log.h"

enum ThreadMode{
    THREAD_CHILD,THREAD_MAIN
};

class MyJNICall {
public:
    JNIEnv *jniEnv;
    JavaVM *javaVM;
    jobject jPlayerObj;
    jmethodID jPlayerErrorMid;
    jmethodID jPlayerPreparedMid;

public:
    MyJNICall(JNIEnv *jniEnv, JavaVM *javaVM, jobject jPlayerObj);
    ~MyJNICall();

public:
    void callPlayerError(ThreadMode threadMode, int code, const char *msg);
    void callPlayerPrepared(ThreadMode mode);
};


#endif //NDKDEMO_MYJNICALL_H
