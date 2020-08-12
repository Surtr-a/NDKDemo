//
// Created by jiuxs on 20-8-8.
// AVPacket 队列
//

#include "MyPacketQueue.h"

// 生产者消费者模式实现 Packet 队列
MyPacketQueue::MyPacketQueue() {
    avPacketQueue = new std::queue<AVPacket *>();
    pthread_mutex_init(&packetMutex, nullptr);
    pthread_cond_init(&packetCond, nullptr);
}

MyPacketQueue::~MyPacketQueue() {
    if (avPacketQueue != nullptr) {
        clear();
        delete avPacketQueue;
        avPacketQueue = nullptr;
    }
    pthread_mutex_destroy(&packetMutex);
    pthread_cond_destroy(&packetCond);
}

void MyPacketQueue::push(AVPacket *avPacket) {
    pthread_mutex_lock(&packetMutex);
    avPacketQueue->push(avPacket);
    pthread_cond_signal(&packetCond);
    pthread_mutex_unlock(&packetMutex);
}

AVPacket *MyPacketQueue::pop() {
    AVPacket *avPacket;
    pthread_mutex_lock(&packetMutex);
    while (avPacketQueue->empty()) {
        pthread_cond_wait(&packetCond, &packetMutex);
    }
    avPacket = avPacketQueue->front();
    avPacketQueue->pop();
    pthread_mutex_unlock(&packetMutex);
    return avPacket;
}

void MyPacketQueue::clear() {
    AVPacket *avPacket;
    while ((avPacket = avPacketQueue->front()) != nullptr) {
        av_packet_free(&avPacket);
        avPacket = nullptr;
        avPacketQueue->pop();
    }
}
