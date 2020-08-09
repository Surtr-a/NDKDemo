//
// Created by jiuxs on 20-8-4.
//

#include "MyJNICall.h"

MyJNICall::MyJNICall(JNIEnv *jniEnv, JavaVM *javaVM, jobject jPlayerObj) {
    this->jniEnv = jniEnv;
    this->javaVM = javaVM;
    // 创建了新线程，需要创建一个全局变量
    this->jPlayerObj = jniEnv->NewGlobalRef(jPlayerObj);

    jclass jPlayerClass = jniEnv->GetObjectClass(jPlayerObj);
    jPlayerErrorMid = jniEnv->GetMethodID(jPlayerClass, "onError", "(ILjava/lang/String;)V");
    jPlayerPreparedMid = jniEnv->GetMethodID(jPlayerClass, "onPrepared", "()V");
}

MyJNICall::~MyJNICall() {
    jniEnv->DeleteGlobalRef(jPlayerObj);
}

void MyJNICall::callPlayerError(ThreadMode threadMode, int code, const char *msg) {
    if (threadMode == THREAD_MAIN) {
        jstring jMsg = jniEnv->NewStringUTF(msg);
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
        jniEnv->DeleteLocalRef(jMsg);
    } else {    // 子线程
        // 获取当前线程的 env
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOGE("get child thread JNIEnv failed");
            return;
        }

        jstring jMsg = env->NewStringUTF(msg);
        env->CallVoidMethod(jPlayerObj, jPlayerErrorMid, code, jMsg);
        env->DeleteLocalRef(jMsg);

        javaVM->DetachCurrentThread();
    }
}

void MyJNICall::callPlayerPrepared(ThreadMode threadMode) {
    if (threadMode == THREAD_MAIN) {
        jniEnv->CallVoidMethod(jPlayerObj, jPlayerPreparedMid);
    } else {    // 子线程
        // 获取当前线程的 env
        JNIEnv *env;
        if (javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
            LOGE("get child thread JNIEnv failed");
            return;
        }

        env->CallVoidMethod(jPlayerObj, jPlayerPreparedMid);

        javaVM->DetachCurrentThread();
    }
}
