//
// Created by jiuxs on 20-8-4.
//

#ifndef NDKDEMO_MYJNICALL_H
#define NDKDEMO_MYJNICALL_H


#include "../../../../../../../../../opt/android-ndk/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/jni.h"

class MyJNICall {
    JNIEnv *jniEnv;
    JavaVM *javaVM;
    jobject jAudioTrackObj;
    jmethodID jAudioTrackWriteMid;
public:
    MyJNICall(JNIEnv *jniEnv, JavaVM *javaVM);
    ~MyJNICall();

private:
    void initCreateAudioTrack();

public:
    void callAudioTrackWrite(jbyteArray audioData, int offsetIntByte, int sizeIntByte);
};


#endif //NDKDEMO_MYJNICALL_H
