//
// Created by jiuxs on 20-8-11.
//

#ifndef NDKDEMO_MYMEDIA_H
#define NDKDEMO_MYMEDIA_H

#include "../audio/MyJNICall.h"
#include "../audio/MyPacketQueue.h"
#include "../audio/MyPlayerStatus.h"
#include "../android_log.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};

// 音视频播放基类
class MyMedia {
public:
    int streamIndex = -1;
    AVCodecContext *avCodecContext = nullptr;
    MyPacketQueue *myPacketQueue = nullptr;
    MyPlayerStatus *myPlayerStatus = nullptr;
    MyJNICall *myJniCall = nullptr;
    // 视频时长
    int duration = 0;
    // 当前进度
    double currentTime = 0;
    // 上次更新时间
    double lastUpdateTime = 0;
    AVRational timeBase{};

public:
    MyMedia(int streamIndex, MyJNICall *myJniCall, MyPlayerStatus *myPlayerState);
    ~MyMedia();

public:
    virtual void play() = 0;    // 纯虚函数，必须重写
    void analysisStream(ThreadMode threadMode, AVFormatContext *avFormatContext);
    virtual void privateAnalysisStream(ThreadMode threadMode, AVFormatContext *avFormatContext) = 0;
    void callPlayerJNIError(ThreadMode threadMode, int code, const char *msg);

private:
    void release();
    void publicAnalysisStream(ThreadMode threadMode, AVFormatContext *avFormatContext);
};


#endif //NDKDEMO_MYMEDIA_H
