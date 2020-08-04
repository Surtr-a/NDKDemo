//
// Created by 30399 on 2020/7/30.
//

#ifndef NDKDEMO_ANDROID_LOG_CPP
#define NDKDEMO_ANDROID_LOG_CPP

#include <android/log.h>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// 输出采样率
#define AUDIO_SAMPLE_RATE 44100
// 每一帧大小，采样率*16bit采样格式*左右双声道
#define AUDIO_SAMPLES_SIZE_PER_CHANNEL AUDIO_SAMPLE_RATE * 4

#endif //NDKDEMO_ANDROID_LOG_CPP
