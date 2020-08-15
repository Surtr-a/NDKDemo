//
// Created by jiuxs on 20-8-7.
//

#ifndef NDKDEMO_MYAUDIO_H
#define NDKDEMO_MYAUDIO_H

#include <pthread.h>
#include "MyJNICall.h"
#include "../android_log.h"
#include "MyPacketQueue.h"
#include "MyPlayerStatus.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "../base/MyMedia.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};

class MyAudio : public MyMedia {
public:
    AVFormatContext *avFormatContext = nullptr;
    SwrContext *swrContext = nullptr;
    uint8_t *resampleOutBuffer = nullptr;

public:
    MyAudio(int audioStreamIndex, MyJNICall *myJniCall, MyPlayerStatus *myPlayerStatus);

    ~MyAudio();

public:
    void play() override;

    void initCreateOpenSLES();

    int resampleAudio();

    void privateAnalysisStream(ThreadMode threadMode, AVFormatContext *avFormatContext) override;

private:
    void release();
};


#endif //NDKDEMO_MYAUDIO_H
