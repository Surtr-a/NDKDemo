//
// Created by 30399 on 2020/7/30.
//

#ifndef NDKDEMO_ANDROID_LOG_CPP
#define NDKDEMO_ANDROID_LOG_CPP

#include <android/log.h>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

#endif //NDKDEMO_ANDROID_LOG_CPP
