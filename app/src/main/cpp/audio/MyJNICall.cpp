//
// Created by jiuxs on 20-8-4.
//

#include "MyJNICall.h"
#include "../android_log.h"

MyJNICall::MyJNICall(JNIEnv *jniEnv, JavaVM *javaVM) {
    this->jniEnv = jniEnv;
    this->javaVM = javaVM;
    initCreateAudioTrack();
}

void MyJNICall::initCreateAudioTrack() {
    jclass audio_track_class = jniEnv->FindClass("android/media/AudioTrack");
    jmethodID construct_mid = jniEnv->GetMethodID(audio_track_class, "<init>", "(IIIIII)V");

    int channelConfig = (0x4 | 0x8);
    int audioFormat = 2;
    int sampleRateInHz = AUDIO_SAMPLE_RATE;

    // 调用 getMinBufferSize()
    jmethodID  getMinBufferSizeMid = jniEnv->GetStaticMethodID(audio_track_class, "getMinBufferSize", "(III)I");
    int minBufferSizeInByte = jniEnv->CallStaticIntMethod(audio_track_class, getMinBufferSizeMid, sampleRateInHz, channelConfig, audioFormat);
    jAudioTrackObj = jniEnv->NewObject(audio_track_class, construct_mid, 3, sampleRateInHz, channelConfig, audioFormat, minBufferSizeInByte, 1);

    // 调用 play()
    jmethodID audio_track_play_mid = jniEnv->GetMethodID(audio_track_class, "play", "()V");
    jniEnv->CallVoidMethod(jAudioTrackObj, audio_track_play_mid);

    // write()
    jAudioTrackWriteMid = jniEnv->GetMethodID(audio_track_class, "write", "([BII)I");
}

void MyJNICall::callAudioTrackWrite(jbyteArray audioData, int offsetIntByte, int sizeIntByte) {
    jniEnv->CallIntMethod(jAudioTrackObj, jAudioTrackWriteMid, audioData, offsetIntByte, sizeIntByte);
}

MyJNICall::~MyJNICall() {
    jniEnv->DeleteLocalRef(jAudioTrackObj);
}
