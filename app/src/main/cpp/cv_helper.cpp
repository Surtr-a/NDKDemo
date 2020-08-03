#include <android/bitmap.h>
#include <jni.h>
#include "include/opencv2/core/mat.hpp"
#include "cv_helper.h"

int cv_helper::bitmap2mat(JNIEnv *env, jobject &bitmap, cv::Mat &dst) {
    AndroidBitmapInfo bitmapInfo;
    int getInfoRes = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (getInfoRes < 0) {
        return getInfoRes;
    }

    void *pixels;
    int lockPixelsRes = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (lockPixelsRes < 0) {
        return lockPixelsRes;
    }

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        //  ANDROID_BITMAP_FORMAT_RGBA_8888 -> CV_8UC4
        dst.create(bitmapInfo.height, bitmapInfo.width, CV_8UC4);
        dst.data = reinterpret_cast<uchar *>(pixels);
    } else if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        dst.create(bitmapInfo.height, bitmapInfo.width, CV_8UC2);
        dst.data = reinterpret_cast<uchar *>(pixels);
    } else {
        return -1;
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;
}

int cv_helper::mat2bitmap(JNIEnv *env, cv::Mat &src, jobject &bitmap) {
    AndroidBitmapInfo bitmapInfo;
    int getInfoRes = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (getInfoRes < 0) {
        return getInfoRes;
    }

    void *pixels;
    int lockPixelsRes = AndroidBitmap_lockPixels(env, bitmap, &pixels);
    if (lockPixelsRes < 0) {
        return lockPixelsRes;
    }

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        cv::Mat temp(bitmapInfo.height, bitmapInfo.width, CV_8UC4, pixels);
        if (src.type() == CV_8UC1) {
            cvtColor(src, temp, cv::COLOR_GRAY2RGBA);
        } else if (src.type() == CV_8UC3) {
            cvtColor(src, temp, cv::COLOR_RGB2RGBA);
        } else if (src.type() == CV_8UC4) {
            src.copyTo(temp);
        }
    } else if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        cv::Mat temp(bitmapInfo.height, bitmapInfo.width, CV_8UC2, pixels);
        if (src.type() == CV_8UC1) {
            cvtColor(src, temp, cv::COLOR_GRAY2BGR565);
        } else if (src.type() == CV_8UC3) {
            cvtColor(src, temp, cv::COLOR_RGB2BGR565);
        } else if (src.type() == CV_8UC4) {
            cvtColor(src, temp, cv::COLOR_RGBA2BGR565);
        }
    } else {
        return -1;
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    return 0;
}
