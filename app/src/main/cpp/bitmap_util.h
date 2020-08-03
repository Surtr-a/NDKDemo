//
// Created by 30399 on 2020/7/2.
//

#ifndef NDKDEMO_BITMAP_UTIL_H
#define NDKDEMO_BITMAP_UTIL_H

#include <jni.h>

class bitmap_util {
public:
    static jobject create_bitmap(JNIEnv *env, int width, int height, int type);
};


#endif //NDKDEMO_BITMAP_UTIL_H
