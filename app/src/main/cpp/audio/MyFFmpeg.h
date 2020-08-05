//
// Created by jiuxs on 20-8-4.
//

#ifndef NDKDEMO_MYFFMPEG_H
#define NDKDEMO_MYFFMPEG_H

#include "MyJNICall.h"
extern "C" {
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
};

class MyFFmpeg {
public:
    AVFormatContext *avFormatContext = nullptr;
    AVCodecContext *avCodecContext = nullptr;
    SwrContext *swrContext = nullptr;
    uint8_t *out_buffer = nullptr;
    const char *url = nullptr;
    MyJNICall *myJNICall = nullptr;
public:
    MyFFmpeg(MyJNICall *myJnNICall1, const char *url);
    ~MyFFmpeg();
public:
    void play();
    void release();
    void callPlayerJNIError(int code, const char *msg);
};


#endif //NDKDEMO_MYFFMPEG_H
