//
// Created by jiuxs on 20-8-4.
//

#include "MyFFmpeg.h"

// FFmpeg 解封装
MyFFmpeg::MyFFmpeg(MyJNICall *myJnNICall1, char *url) {
    this->myJNICall = myJnNICall1;
    // 复制一份，避免在外面被销毁
    this->url = static_cast<char *>(malloc(strlen(url) + 1));
    memcpy(this->url, url, strlen(url) + 1);

    myPlayerStatus = new MyPlayerStatus();
}

MyFFmpeg::~MyFFmpeg() {
    release();
}

void *threadReadPacket(void *context) {
    auto *myFFmpeg = static_cast<MyFFmpeg *>(context);
    while (!myFFmpeg->myPlayerStatus->isExit) {
        AVPacket *avPacket = av_packet_alloc();
        if (av_read_frame(myFFmpeg->avFormatContext, avPacket) >= 0) {
            if (avPacket->stream_index == myFFmpeg->myAudio->streamIndex) {
                // 将 Packet 放入队列
                myFFmpeg->myAudio->myPacketQueue->push(avPacket);
            } else {
                av_packet_free(&avPacket);
            }
        } else {
            av_packet_free(&avPacket);
        }
    }
    return nullptr;
}

void MyFFmpeg::play() {
    // 解码线程
    pthread_t readPacketThreadT;
    pthread_create(&readPacketThreadT, nullptr, threadReadPacket, this);
    pthread_detach(readPacketThreadT);

    if (myAudio != nullptr) {
        myAudio->play();
    }
}

void MyFFmpeg::release() {
    avformat_network_deinit();
    if (avFormatContext != nullptr) {
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        avFormatContext = nullptr;
    }
    if (url != nullptr) {
        free(url);
        url = nullptr;
    }
    if (myPlayerStatus) {
        delete myPlayerStatus;
        myPlayerStatus = nullptr;
    }
    if (myAudio) {
        delete myAudio;
        myAudio = nullptr;
    }
}

void MyFFmpeg::callPlayerJNIError(ThreadMode threadMode, int code, const char *msg) {
    release();
    myJNICall->callPlayerError(threadMode, code, msg);
}

void MyFFmpeg::prepare() {
    prepare(THREAD_MAIN);
}

void *threadPrepare(void *context) {
    auto *myFFmpeg = (MyFFmpeg *) context;
    myFFmpeg->prepare(THREAD_CHILD);
    return nullptr;
}

void MyFFmpeg::prepareAsync() {
    pthread_t prepareThreadT;
    pthread_create(&prepareThreadT, nullptr, threadPrepare, this);
    pthread_detach(prepareThreadT);
}

void MyFFmpeg::prepare(ThreadMode threadMode) {
    avformat_network_init();

    // 打开音频文件
    avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, url, nullptr, nullptr) != 0) {
        LOGE("打开音频失败");
        callPlayerJNIError(threadMode, OPEN_INPUT_ERROR_CODE, "open input failed");
        return;
    }

    // 获取音频流信息
    if (avformat_find_stream_info(avFormatContext, nullptr) < 0) {
        LOGE("获取音频流失败");
        callPlayerJNIError(threadMode, FIND_STREAM_INFO_ERROR_CODE, "find stream info failed");
        return;
    }

    // 查找音频流索引 Index
    int audio_stream_index;
    audio_stream_index = av_find_best_stream(avFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                             nullptr, 0);
    if (audio_stream_index < 0) {
        LOGE("查找音频索引流失败");
        callPlayerJNIError(threadMode, FIND_BEST_STREAM_ERROR_CODE, "find best stream failed");
        return;
    }

    myAudio = new MyAudio(audio_stream_index, myJNICall, myPlayerStatus);
    // 解码流
    myAudio->analysisStream(threadMode, avFormatContext);
    // 准备完成
    myJNICall->callPlayerPrepared(threadMode);
}

