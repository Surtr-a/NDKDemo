//
// Created by jiuxs on 20-8-7.
//

#include "MyAudio.h"

// 解码和播放
MyAudio::MyAudio(int audioStreamIndex, MyJNICall *myJniCall, MyPlayerStatus *myPlayerStatus)
        : MyMedia(audioStreamIndex, myJniCall, myPlayerStatus) {
}

void *threadPlay(void *context) {
    auto *myAudio = static_cast<MyAudio *>(context);
    myAudio->initCreateOpenSLES();
    return nullptr;
}

void MyAudio::play() {
    // 播放线程
    pthread_t playThreadT;
    pthread_create(&playThreadT, nullptr, threadPlay, this);
    pthread_detach(playThreadT);
}

int MyAudio::resampleAudio() {
    int dataSize = 0;
    AVPacket *avPacket = nullptr;
    AVFrame *avFrame = av_frame_alloc();

    while (!myPlayerStatus->isExit) {
        avPacket = myPacketQueue->pop();
        // 解码音频类型的 Packet
        avcodec_send_packet(avCodecContext, avPacket);
        if (avcodec_receive_frame(avCodecContext, avFrame) == 0) {
            if (resampleOutBuffer == nullptr) {
                resampleOutBuffer = static_cast<uint8_t *>(malloc(avFrame->nb_samples));
            }
            // 重采样, 返回重采样的个数，即 pFrame->nb_sample
            dataSize = swr_convert(swrContext, &resampleOutBuffer, avFrame->nb_samples,
                                   (const uint8_t **) avFrame->data, avFrame->nb_samples);

            // 获取音频当前播放的今进度
            double time = av_frame_get_best_effort_timestamp(avFrame) * av_q2d(timeBase);
            if (time > currentTime) {
                currentTime = time;
            }
            break;
        }
        // 解引用
        av_packet_unref(avPacket);
        av_frame_unref(avFrame);
    }
    // 解引用，销毁内存，指针置空
    av_packet_free(&avPacket);
    av_frame_free(&avFrame);
    return dataSize * 2 * 2;
}

// OpenSL ES 播放回调
void playerCallback(SLAndroidSimpleBufferQueueItf caller, void *pContext) {
    auto *myAudio = (MyAudio *) pContext;
    // 先重采样再加入播放队列
    int dataSize = myAudio->resampleAudio();
    (*caller)->Enqueue(caller, myAudio->resampleOutBuffer, dataSize);
}

void MyAudio::initCreateOpenSLES() {
    SLObjectItf engineObject = nullptr;
    SLEngineItf engineEngine;
    slCreateEngine(&engineObject, 0, nullptr, 0, nullptr, nullptr);
    // realize the engine
    (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    // get the engine interface, which is needed in order to create other objects
    (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    // 3.2 设置混音器
    static SLObjectItf outputMixObject = nullptr;
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
    (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = nullptr;
    (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                     &outputMixEnvironmentalReverb);
    SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
    (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(outputMixEnvironmentalReverb,
                                                                      &reverbSettings);
    // 3.3 创建播放器
    SLObjectItf pPlayer = nullptr;
    SLPlayItf pPlayItf = nullptr;
    SLDataLocator_AndroidSimpleBufferQueue simpleBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM formatPcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN};
    SLDataSource audioSrc = {&simpleBufferQueue, &formatPcm};
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&outputMix, nullptr};
    SLInterfaceID interfaceIds[3] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_PLAYBACKRATE};
    SLboolean interfaceRequired[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    (*engineEngine)->CreateAudioPlayer(engineEngine, &pPlayer, &audioSrc, &audioSnk, 3,
                                       interfaceIds, interfaceRequired);
    (*pPlayer)->Realize(pPlayer, SL_BOOLEAN_FALSE);
    (*pPlayer)->GetInterface(pPlayer, SL_IID_PLAY, &pPlayItf);
    // 3.4 设置缓存队列和回调函数
    SLAndroidSimpleBufferQueueItf playerBufferQueue;
    (*pPlayer)->GetInterface(pPlayer, SL_IID_BUFFERQUEUE, &playerBufferQueue);
    // 每次回调 this 会带给 playerCallback 里的 context
    (*playerBufferQueue)->RegisterCallback(playerBufferQueue, playerCallback, this);
    // 3.5 设置播放状态
    (*pPlayItf)->SetPlayState(pPlayItf, SL_PLAYSTATE_PLAYING);
    // 3.6 调用回调函数
    playerCallback(playerBufferQueue, this);
}

MyAudio::~MyAudio() {
    release();
}

void MyAudio::release() {
    if (resampleOutBuffer) {
        free(resampleOutBuffer);
        resampleOutBuffer = nullptr;
    }
    if (swrContext) {
        swr_free(&swrContext);
        swrContext = nullptr;
    }
}

void MyAudio::privateAnalysisStream(ThreadMode threadMode, AVFormatContext *myFormatContext) {
    // 输入采样格式
    enum AVSampleFormat in_sample_fmt = avCodecContext->sample_fmt;
    // 输出采样格式（16bit PCM）
    enum AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    // 输入采样率
    int in_sample_rate = avCodecContext->sample_rate;
    // 输出采样率
    int out_sample_rate = AUDIO_SAMPLE_RATE;
    // 获取输入的声道布局
    // 根据声道个数获取默认的声道布局（2个声道，默认立体声 stereo）
    uint64_t in_ch_layout = avCodecContext->channel_layout;
    // 输出声道布局（立体声）
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    // 重采样上下文
    swrContext = swr_alloc();
    swrContext = swr_alloc_set_opts(nullptr, out_ch_layout, out_sample_fmt, out_sample_rate,
                                    in_ch_layout, in_sample_fmt, in_sample_rate, 0,
                                    nullptr);
    if (swrContext == nullptr) {
        LOGE("获取重采样上下文失败");
        callPlayerJNIError(threadMode, ALLOC_SET_OPTS_ERROR_CODE, "alloc set opts failed");
        return;
    }
    if (swr_init(swrContext) < 0) {
        LOGE("初始化重采样上下文失败");
        callPlayerJNIError(threadMode, SWR_INIT_ERROR_CODE, "init swr failed");
        return;
    }
}
