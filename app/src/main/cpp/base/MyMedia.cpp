//
// Created by jiuxs on 20-8-11.
//

#include "MyMedia.h"

MyMedia::MyMedia(int streamIndex, MyJNICall *myJNICall, MyPlayerStatus *myPlayerStatus) {
    this->streamIndex = streamIndex;
    this->myJniCall = myJNICall;
    this->myPlayerStatus = myPlayerStatus;
    myPacketQueue = new MyPacketQueue();
}

MyMedia::~MyMedia() {
    release();
}

void MyMedia::analysisStream(ThreadMode threadMode, AVFormatContext *avFormatContext) {
    publicAnalysisStream(threadMode, avFormatContext);
    privateAnalysisStream(threadMode, avFormatContext);
}

void MyMedia::release() {
    if (myPacketQueue) {
        delete myPacketQueue;
        myPacketQueue = nullptr;
    }
    if (avCodecContext) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
        avCodecContext = nullptr;
    }
}

void MyMedia::callPlayerJNIError(ThreadMode threadMode, int code, const char *msg) {
    release();
    myJniCall->callPlayerError(threadMode, code, msg);
}

void MyMedia::publicAnalysisStream(ThreadMode threadMode, AVFormatContext *avFormatContext) {
    // 获取解码器
    AVCodecParameters *aVCodecParameters = avFormatContext->streams[streamIndex]->codecpar;
    AVCodec *avCodec = avcodec_find_decoder(aVCodecParameters->codec_id);
    if (avCodec == nullptr) {
        LOGE("获取解码器失败");
        callPlayerJNIError(threadMode, FIND_DECODER_ERROR_CODE, "find decoder failed");
        return;
    }

    // 获取解码器上下文
    avCodecContext = avcodec_alloc_context3(avCodec);
    if (avcodec_parameters_to_context(avCodecContext, aVCodecParameters) < 0) {
        LOGE("获取解码器上下文失败");
        callPlayerJNIError(threadMode, PARAMETERS_TO_CONTEXT_ERROR_CODE,
                           "parameters to context failed");
        return;
    }

    // 打开解码器
    if (avcodec_open2(avCodecContext, avCodec, nullptr) != 0) {
        LOGE("打开解码器失败");
        callPlayerJNIError(threadMode, OPEN_DECODER_ERROR_CODE, "open decoder failed");
        return;
    }
    duration = avFormatContext->duration;
    timeBase = avFormatContext->streams[streamIndex]->time_base;
}
