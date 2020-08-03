//
// Created by 30399 on 2020/7/4.
//

#include <jni.h>
#include "opencv2/opencv.hpp"
#include "cv_helper.h"
#include <android/log.h>

using namespace cv;

// 取 bins=1 提取直方图
void calcHist(const Mat &mat, Mat &hist) {
    // 存储 int 类型数据
    hist.create(1, 256, CV_32S);
    // 初始化
    for (int i = 0; i < hist.cols; ++i) {
        hist.at<int>(0, i) = 0;
    }

    // 统计
    for (int row = 0; row < mat.rows; ++row) {
        for (int col = 0; col < mat.cols; ++col) {
            int index = mat.at<uchar>(row, col);
            hist.at<int>(0, index) += 1;
        }
    }
}

void normalize(const Mat &src, Mat &dst, int n_max) {
    // 找最大值
    int max_value = 0;
    for (int row = 0; row < src.rows; ++row) {
        for (int col = 0; col < src.cols; ++col) {
            int value = src.at<int>(row, col);
            max_value = max(value, max_value);
        }
    }

    dst.create(src.size(), src.type());
    for (int row = 0; row < src.rows; ++row) {
        for (int col = 0; col < src.cols; ++col) {
            int value = src.at<int>(row, col);
            dst.at<int>(row, col) = value * (1.0 / max_value) * n_max;
        }
    }
}

void equalizeHist(const Mat &src, Mat &dst) {
    Mat hist;
    calcHist(src, hist);

    Mat prob_mat(hist.size(), CV_32FC1);    // 累加概率数组
    float image_size = src.cols * src.rows; // 像素点总数
    float prob_sum = 0;
    for (int i = 0; i < hist.cols; ++i) {
        float times = hist.at<int>(0, i);   // 出现次数
        float prob = times / image_size;    // 出现概率
        prob_sum += prob;   // 累加概率
        prob_mat.at<float>(0, i) = prob_sum;
    }

    dst.create(src.size(), src.type());
    for (int row = 0; row < src.rows; ++row) {
        for (int col = 0; col < src.cols; ++col) {
            uchar pixel = src.at<uchar>(row, col);
            dst.at<uchar>(row, col) = prob_mat.at<float>(0, pixel) * 255;
        }
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_equalizeHist(JNIEnv *env, jobject thiz,
                                                                   jobject src) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);
    Mat gray;
    cvtColor(mat, gray, COLOR_BGRA2GRAY);
    Mat dst;
    equalizeHist(gray, dst);
    cv_helper::mat2bitmap(env, dst, src);
    return 1;
}