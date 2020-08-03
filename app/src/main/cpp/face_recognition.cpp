//
// Created by 30399 on 2020/7/10.
//
#include <jni.h>
#include <vector>
#include "opencv2/opencv.hpp"
#include "cv_helper.h"

using namespace cv;
using namespace std;

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_hogBitmap(JNIEnv *env, jobject thiz,
                                                                jobject src) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);
//    Mat dst, dst_gray;
//    resize(mat, dst, Size(64, 128));    // 缩放
//    cvtColor(dst, dst_gray, COLOR_BGRA2GRAY);
//
//    HOGDescriptor hogDescriptor;
//    vector<float> descriptors;
//    vector<Point> locations;
//    hogDescriptor.compute(dst_gray, descriptors, Size(), Size(), locations);

    Mat bgr;
    cvtColor(mat, bgr, COLOR_BGRA2BGR);
    HOGDescriptor descriptor;
    descriptor.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    vector<Rect> foundLocations;
    // 检测
    descriptor.detectMultiScale(bgr, foundLocations, 0, Size(14, 14));
    // 标记
    for (const auto & foundLocation : foundLocations) {
        rectangle(mat, foundLocation, Scalar(255, 0, 0, 255), 2, LINE_AA);
    }

    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_lbpBitmap(JNIEnv *env, jobject thiz,
                                                                jobject src) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);

    // 核大小
    int size = 3;
    // 边界补充
    int radius = size / 2;
    Mat mat_exp;
    copyMakeBorder(mat, mat_exp, radius, radius, radius, radius, BORDER_DEFAULT);
    // 转灰度
    Mat gray;
    cvtColor(mat_exp, gray, COLOR_BGRA2GRAY);

    // 特征数据结果
    Mat result = Mat::zeros(Size(mat.cols, mat.rows), CV_8UC1);

    for (int row = 1; row < gray.rows - 1; ++row) { // 3*3
        for (int col = 1; col < gray.cols - 1; ++col) {
            uchar pixel = gray.at<uchar>(row, col);
            int rPixel = 0;
            rPixel |= (pixel <= gray.at<uchar>(row - 1, col - 1)) << 0; // 使用或运算效率更高
            rPixel |= (pixel <= gray.at<uchar>(row - 1, col)) << 1;
            rPixel |= (pixel <= gray.at<uchar>(row - 1, col + 1)) << 2;
            rPixel |= (pixel <= gray.at<uchar>(row, col - 1)) << 7;
            rPixel |= (pixel <= gray.at<uchar>(row, col + 1)) << 3;
            rPixel |= (pixel <= gray.at<uchar>(row + 1, col - 1)) << 6;
            rPixel |= (pixel <= gray.at<uchar>(row + 1, col)) << 5;
            rPixel |= (pixel <= gray.at<uchar>(row + 1, col + 1)) << 4;

            result.at<uchar>(row - 1, col - 1) = rPixel;
        }
    }

    cv_helper::mat2bitmap(env, result, src);
    return 1;
}
