//
// Created by jiuxs on 20-8-12.
//

#include "MyVideo.h"

MyVideo::MyVideo(int streamIndex, MyJNICall *myJNICall, MyPlayerStatus *myPlayerStatus,
                 MyAudio *myAudio) : MyMedia(streamIndex, myJNICall, myPlayerStatus) {
    this->myAudio = myAudio;
}

MyVideo::~MyVideo() {
    release();
}

void *threadVideoPlay(void *context) {
    auto *myVideo = static_cast<MyVideo *>(context);

    // 获取当前线程的 env
    JNIEnv *env;
    if (myVideo->myJniCall->javaVM->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("get child thread jniEnv error");
        return nullptr;
    }
    // 获取窗体
    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, myVideo->surface);
    myVideo->myJniCall->javaVM->DetachCurrentThread();

    // 设置缓存区数据
    ANativeWindow_setBuffersGeometry(aNativeWindow, myVideo->avCodecContext->width,
                                     myVideo->avCodecContext->height, WINDOW_FORMAT_RGBA_8888);

    // 缓冲区 buffer
    ANativeWindow_Buffer outBuffer;

    AVPacket *avPacket = nullptr;
    AVFrame *avFrame = av_frame_alloc();

    while (!myVideo->myPlayerStatus->isExit) {
        avPacket = myVideo->myPacketQueue->pop();
        // 解码视频类型的 Packet
        avcodec_send_packet(myVideo->avCodecContext, avPacket);
        if (avcodec_receive_frame(myVideo->avCodecContext, avFrame) == 0) {
            // 一般从 AVFrame 中拿到的数据是 yuv420p 的，需要进行转化
            sws_scale(myVideo->swsContext, (const uint8_t *const *) avFrame->data,
                      avFrame->linesize, 0, myVideo->avCodecContext->height, myVideo->avFrame->data,
                      myVideo->avFrame->linesize);

            // 播放之前的同步
            uint8_t frameSleepTime = myVideo->getFrameSleepTime(avFrame);
            av_usleep(frameSleepTime * 1000000);

            // 把数据推到缓冲区
            ANativeWindow_lock(aNativeWindow, &outBuffer, nullptr);
            memcpy(outBuffer.bits, myVideo->frameBuffer, myVideo->frameSize);
            ANativeWindow_unlockAndPost(aNativeWindow);
        }
        // 解引用
        av_packet_unref(avPacket);
        av_frame_unref(avFrame);
    }
    // 解引用，销毁内存，指针置空
    av_packet_free(&avPacket);
    av_frame_free(&avFrame);
    return nullptr;
}

void MyVideo::play() {
    pthread_t playThreadT;
    pthread_create(&playThreadT, nullptr, threadVideoPlay, this);
    pthread_detach(playThreadT);
}

void MyVideo::setSurface(JNIEnv *env, jobject jSurface) {
    this->surface = env->NewGlobalRef(jSurface);
}

double MyVideo::getFrameSleepTime(AVFrame *frame) {
    double time = av_frame_get_best_effort_timestamp(avFrame) * av_q2d(timeBase);
    if (time > currentTime) {
        currentTime = time;
    }
    double diffTime = myAudio->currentTime - currentTime;
    LOGE("currentTime=%lf", currentTime);
    LOGE("diffTime=%lf", diffTime);
    // 视频同步音频进度
    if (diffTime > 0.016 || diffTime < -0.016) {
        // 第一次控制
        delayTime = delayTime * 2 / 3;
        // 第二次控制
        if (delayTime < defaultDelayTime / 2) {
            delayTime = defaultDelayTime * 2 / 3;
        } else if (delayTime > defaultDelayTime * 2) {
            delayTime = defaultDelayTime * 3 / 2;
        }
    }

    // 第三次控制（异常情况）
    if (diffTime >= 0.25) {
        delayTime = 0;
    } else if (diffTime <= -0.25) {
        delayTime = -defaultDelayTime * 2;
    }
    return delayTime;
}

void MyVideo::privateAnalysisStream(ThreadMode threadMode, AVFormatContext *avFormatContext) {
    swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                avCodecContext->pix_fmt,
                                avCodecContext->width, avCodecContext->height, AV_PIX_FMT_RGBA,
                                SWS_BILINEAR,
                                nullptr, nullptr, nullptr);
    avFrame = av_frame_alloc();
    frameSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, avCodecContext->width,
                                         avCodecContext->height, 1);
    frameBuffer = static_cast<uint8_t *>(malloc(frameSize));
    av_image_fill_arrays(avFrame->data, avFrame->linesize, frameBuffer, AV_PIX_FMT_RGBA,
                         avCodecContext->width, avCodecContext->height, 1);
    int num = avFormatContext->streams[streamIndex]->avg_frame_rate.num;
    int den = avFormatContext->streams[streamIndex]->avg_frame_rate.den;
    if (num && den) {
        defaultDelayTime = 1.0f * den / num;
    }
}

void MyVideo::release() {
    if (swsContext) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }
    if (frameBuffer) {
        free(frameBuffer);
        frameBuffer = nullptr;
    }
    if (avFrame) {
        av_frame_free(&avFrame);
        avFrame = nullptr;
    }
    if (myJniCall) {
        myJniCall->jniEnv->DeleteGlobalRef(surface);
    }
}
