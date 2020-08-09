//
// Created by jiuxs on 20-8-7.
//

#ifndef NDKDEMO_MYAUDIO_H
#define NDKDEMO_MYAUDIO_H

#include <pthread.h>
#include "MyJNICall.h"
#include "../android_log.h"
#include "MyPacketQueue.h"
#include "MyPlayerState.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};

class MyAudio {
public:
    AVFormatContext *avFormatContext = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    SwrContext *swrContext = nullptr;
    uint8_t *resampleOutBuffer = nullptr;
    MyJNICall *myJniCall = nullptr;
    int audioStreamIndex = -1;
    MyPacketQueue *myPacketQueue = nullptr;
    MyPlayerState *myPlayerState = nullptr;

public:
    MyAudio(int audioStreamIndex, MyJNICall *myJniCall, AVFormatContext *avFormatContext);
    ~MyAudio();

public:
    void play();
    void initCreateOpenSLES();
    int resampleAudio();
    void analysisStream(ThreadMode threadMode, AVStream **pAvStream);

private:
    void callPlayerJNIError(ThreadMode threadMode, int code, const char *msg);
    void release();
};


#endif //NDKDEMO_MYAUDIO_H
