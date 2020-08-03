//
// Created by 30399 on 2020/7/2.
//

#include "bitmap_util.h"
#include "opencv2/opencv.hpp"

jobject bitmap_util::create_bitmap(JNIEnv *env, int width, int height, int type) {
    const char *config_name;
    if (type == CV_8UC4) {
        config_name = "ARGB_8888";
    } else if (type == CV_8UC3 || type == CV_8UC1) {
        config_name = "RGB_565";
    } else
        return nullptr;

    jstring configName = env->NewStringUTF(config_name);
    jclass bitmap_config_class = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID create_bitmap_config_mid = env->GetStaticMethodID(bitmap_config_class, "valueOf", "(Ljava/lang/Class;Ljava/lang/String;)Ljava/lang/Enum;");
    jobject config = env->CallStaticObjectMethod(bitmap_config_class, create_bitmap_config_mid, bitmap_config_class, configName);

    jclass bitmap_class = env->FindClass("android/graphics/Bitmap");
    jmethodID create_bitmap_mid = env->GetStaticMethodID(bitmap_class, "createBitmap", "(IILandroid/graphics/Bitmap/Config;)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(bitmap_class, create_bitmap_mid, width, height, config);
    return bitmap;
}