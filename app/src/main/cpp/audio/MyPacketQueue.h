//
// Created by jiuxs on 20-8-8.
//

#ifndef NDKDEMO_MYPACKETQUEUE_H
#define NDKDEMO_MYPACKETQUEUE_H

#include <queue>
#include <pthread.h>
extern "C" {
#include "libavcodec/avcodec.h"
};

class MyPacketQueue {
public:
    std::queue<AVPacket *> *avPacketQueue = nullptr;
    pthread_mutex_t packetMutex;
    pthread_cond_t packetCond;

public:
    MyPacketQueue();
    ~MyPacketQueue();

public:
    void push(AVPacket *avPacket);
    AVPacket *pop();
    void clear();
};


#endif //NDKDEMO_MYPACKETQUEUE_H
