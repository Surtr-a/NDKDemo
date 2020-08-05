//
// Created by jiuxs on 20-8-4.
//

#ifndef NDKDEMO_MYJNICALL_H
#define NDKDEMO_MYJNICALL_H


#include "jni.h"

class MyJNICall {
public:
    JNIEnv *jniEnv;
    JavaVM *javaVM;
    jobject jAudioTrackObj;
    jobject jPlayerObj;
    jmethodID jAudioTrackWriteMid;
    jmethodID jPlayerErrorMid;

public:
    MyJNICall(JNIEnv *jniEnv, JavaVM *javaVM, jobject jPlayerObj);
    ~MyJNICall();

private:
    void initCreateAudioTrack();

public:
    void callAudioTrackWrite(jbyteArray audioData, int offsetIntByte, int sizeIntByte);
    void callPlayerError(int code, const char *msg);
};


#endif //NDKDEMO_MYJNICALL_H
