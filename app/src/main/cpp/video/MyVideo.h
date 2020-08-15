//
// Created by jiuxs on 20-8-12.
//

#ifndef NDKDEMO_MYVIDEO_H
#define NDKDEMO_MYVIDEO_H

#include "../base/MyMedia.h"
#include "../audio/MyAudio.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
};

class MyVideo : public MyMedia {
public:
    SwsContext *swsContext = nullptr;
    uint8_t *frameBuffer = nullptr;
    int frameSize{};
    AVFrame *avFrame{};
    jobject surface{};
    MyAudio *myAudio;
    // 视频的延迟时间
    double delayTime = 0;
    double defaultDelayTime = 0.04;

public:
    MyVideo(int streamIndex, MyJNICall *myJNICall, MyPlayerStatus *myPlayerStatus, MyAudio *myAudio);
    ~MyVideo();

public:
    void play() override;

    void setSurface(JNIEnv *env, jobject jSurface);

    double getFrameSleepTime(AVFrame *frame);

    void privateAnalysisStream(ThreadMode threadMode, AVFormatContext *avFormatContext) override;

private:
    void release();
};


#endif //NDKDEMO_MYVIDEO_H
