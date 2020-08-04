//
// Created by 30399 on 2020/7/29.
//

#include <jni.h>
#include <malloc.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
// 缩放
#include "libswscale/swscale.h"
// 重采样
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
}

#include "../android_log.h"
#include "MyJNICall.h"

MyJNICall *myJniCall;

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_MyPlayer_nPlay(JNIEnv *env, jobject thiz, jstring url) {
    const char *url_ = env->GetStringUTFChars(url, nullptr);
    myJniCall = new MyJNICall(env, nullptr);

    // 注册组件，废弃，4.0以上忽略
//    av_register_all();
    avformat_network_init();

    // 打开音频文件
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, url_, nullptr, nullptr) != 0) {
        LOGE("打开音频失败");
        return;
    }

    // 获取音频流信息
    if (avformat_find_stream_info(avFormatContext, nullptr) < 0) {
        LOGE("获取音频流失败");
        return;
    }

    // 查找音频流
    int audio_stream_index = -1;
    audio_stream_index = av_find_best_stream(avFormatContext, AVMediaType::AVMEDIA_TYPE_AUDIO, -1, -1,
                                             nullptr, 0);
    if (audio_stream_index < 0) {
        LOGE("查找音频流失败");
        return;
    }

    // 获取解码器
    AVCodecParameters *aVCodecParameters = avFormatContext->streams[audio_stream_index]->codecpar;
    AVCodec *avCodec = avcodec_find_decoder(aVCodecParameters->codec_id);
    if (avCodec == nullptr) {
        LOGE("获取解码器失败");
        return;
    }

    // 获取解码器上下文
    AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);
    if (avcodec_parameters_to_context(avCodecContext, aVCodecParameters) < 0) {
        LOGE("获取编码器上下文失败");
        return;
    }

    // 打开编码器
    if (avcodec_open2(avCodecContext, avCodec, nullptr) != 0) {
        LOGE("打开编码器失败");
        return;
    }

    SwrContext *swrContext = swr_alloc();

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
    swrContext = swr_alloc_set_opts(nullptr, out_ch_layout, out_sample_fmt, out_sample_rate, in_ch_layout, in_sample_fmt, in_sample_rate, 0,
                       nullptr);
    if (swrContext == nullptr) {
        LOGE("获取重采样上下文失败");
        return;
    }
    if (swr_init(swrContext) < 0) {
        LOGE("初始化重采样上下文失败");
        return;
    }

//    int out_channels = av_get_channel_layout_nb_channels(out_ch_layout);
//    int out_buffer_size = av_samples_get_buffer_size(nullptr, out_channels, aVCodecParameters->frame_size, out_sample_fmt, 0);
    auto *out_buffer = static_cast<uint8_t *>(av_malloc(AUDIO_SAMPLES_SIZE_PER_CHANNEL));

    // 逐帧播放
    jbyteArray audio_sample_array;
    jbyte *audio_sample_byte;
    AVPacket *avPacket = av_packet_alloc();
    AVFrame *avFrame = av_frame_alloc();
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        // 解码音频类型的 Packet
        if (avPacket->stream_index == audio_stream_index) {
            avcodec_send_packet(avCodecContext, avPacket);
            if (avcodec_receive_frame(avCodecContext, avFrame) == 0) {
                // 播放
                // out_buffer 缓冲数据转 byte 数组
                // 获取 sample size
                int out_buffer_size = av_samples_get_buffer_size(nullptr, avFrame->channels,avFrame->nb_samples, avCodecContext->sample_fmt, 0);
                audio_sample_array = env->NewByteArray(out_buffer_size);
                // 获取数组指针
                audio_sample_byte = env->GetByteArrayElements(audio_sample_array, nullptr);
                // 重采样
                swr_convert(swrContext, &out_buffer, out_buffer_size, (const uint8_t **)avFrame->data, avFrame->nb_samples);
                // out_buffer 的数据复制到 sample_byte
                memcpy(audio_sample_byte, out_buffer, out_buffer_size);

                // 同步
                env->ReleaseByteArrayElements(audio_sample_array, audio_sample_byte, 0);

                // AudioTrack.write() PCM 数据
                myJniCall->callAudioTrackWrite(audio_sample_array, 0, out_buffer_size);
                env->DeleteLocalRef(audio_sample_array);
            }
        }
        // 解引用
        av_packet_unref(avPacket);
        av_frame_unref(avFrame);
    }

    // 释放资源
    av_packet_free(&avPacket);
    av_frame_free(&avFrame);
    env->ReleaseByteArrayElements(audio_sample_array, audio_sample_byte, JNI_ABORT);
    av_free(out_buffer);
    delete myJniCall;
    swr_free(&swrContext);
    if (avCodecContext != nullptr) {
        avcodec_close(avCodecContext);
        avcodec_free_context(&avCodecContext);
    }
    if (avFormatContext != nullptr) {
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        avFormatContext = nullptr;
    }
    avformat_network_deinit();

    env->ReleaseStringUTFChars(url, url_);
}