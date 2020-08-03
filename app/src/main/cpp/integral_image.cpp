//
// Created by 30399 on 2020/7/6.
//

#include <jni.h>
#include "opencv2/opencv.hpp"
#include "cv_helper.h"
#include <android/log.h>

using namespace cv;

// 积分图均值模糊
void meanBlur(const Mat &src, Mat &dst, int size) {   // size - 模糊直径
    Mat mat;
    dst.create(src.size(), src.type());
    // 边缘补充
    int radius = size / 2;
    copyMakeBorder(src, mat, radius, radius, radius, radius, BORDER_DEFAULT);

    // 获取积分图
    Mat sum_mat, sqsum_mat;
    integral(mat, sum_mat, sqsum_mat, CV_32S, CV_32S);

    int mat_h = mat.rows;
    int mat_w = mat.cols;
    int area = size * size; // 核大小
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0; // 坐标
    int lt = 0, lb = 0, rt = 0, rb = 0; // 值，左上 左下 右上 右下
    for (int row = radius; row < mat_h - radius; ++row) {   // 从第一个不是补充边缘的像素开始滑动
        y0 = row - radius;
        y1 = row + radius + 1;
        for (int col = radius; col < mat_w - radius; ++col) {
            x0 = col - radius;
            x1 = col + radius + 1;
            for (int i = 0; i < mat.channels(); ++i) {
                // 获取四个点的值
                lt = sum_mat.at<Vec4i>(y0, x0)[i];
                lb = sum_mat.at<Vec4i>(y1, x0)[i];
                rt = sum_mat.at<Vec4i>(y0, x1)[i];
                rb = sum_mat.at<Vec4i>(y1, x1)[i];

                // 求平均值并赋值
                int sum = rb - rt - lb + lt;
                dst.at<Vec4b>(row, col)[i] = sum / area;
            }
        }
    }
}

int getBlockSum(Mat &mat, int x0, int y0, int x1, int y1, int ch) {
    int lt = mat.at<Vec4i>(y0, x0)[ch];
    int lb = mat.at<Vec4i>(y1, x0)[ch];
    int rt = mat.at<Vec4i>(y0, x1)[ch];
    int rb = mat.at<Vec4i>(y1, x1)[ch];

    int sum = rb - rt - lb + lt;
    return sum;
}

float getBlockSqSum(Mat &mat, int x0, int y0, int x1, int y1, int ch) {
    float lt = mat.at<Vec4f>(y0, x0)[ch];
    float lb = mat.at<Vec4f>(y1, x0)[ch];
    float rt = mat.at<Vec4f>(y0, x1)[ch];
    float rb = mat.at<Vec4f>(y1, x1)[ch];

    float sqsum = rb - rt - lb + lt;
    return sqsum;
}

// size - 模糊直径
// sigma - 越大模糊效果越强
void fastBilateralBlur(const Mat &src, Mat &dst, int size, int sigma) {
    Mat mat;
    dst.create(src.size(), src.type());
    // 边缘补充
    int radius = size / 2;
    copyMakeBorder(src, mat, radius, radius, radius, radius, BORDER_DEFAULT);

    // 获取积分图
    Mat sum_mat, sqsum_mat;
    integral(mat, sum_mat, sqsum_mat, CV_32S, CV_32F);  // 方差取float

    int mat_h = mat.rows;
    int mat_w = mat.cols;
    int area = size * size; // 核大小
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0; // 坐标
    for (int row = radius; row < mat_h - radius; ++row) {   // 从第一个不是补充边缘的像素开始滑动
        y0 = row - radius;
        y1 = row + radius + 1;
        for (int col = radius; col < mat_w - radius; ++col) {
            x0 = col - radius;
            x1 = col + radius + 1;
            for (int i = 0; i < mat.channels(); ++i) {
                int sum = getBlockSum(sum_mat, x0, y0, x1, y1, i);
                float sqsum = getBlockSqSum(sqsum_mat, x0, y0, x1, y1, i);

                // 局部平方差
                float diff_sq = (sqsum - (sum * sum) / area) / area;
                float k = diff_sq / (diff_sq + sigma);

                int pixel = src.at<Vec4b>(row, col)[i];
                pixel = (1 - k) * (sum / area) + k * pixel;

                dst.at<Vec4b>(row, col)[i] = pixel;
            }
        }
    }
}

// 皮肤区域检测，返回二值化图像
void skinDetect(const Mat &src, Mat &skinMask) {
    skinMask.create(src.size(), CV_8UC1);
    int rows = src.rows;
    int cols = src.cols;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            Vec4b pixel = src.at<Vec4b>(row, col);
            int b = pixel[0];
            int g = pixel[1];
            int r = pixel[2];

            if (r > 95 &&
                g > 40 &&
                b > 20) {
                skinMask.at<uchar>(row, col) = 255;
            } else {
                skinMask.at<uchar>(row, col) = 0;
            }
        }
    }
}

// 皮肤区域融合
void skinFuse(const Mat &src, const Mat &blur_mat, Mat &dst, const Mat &mask) {
    dst.create(src.size(), src.type());
    GaussianBlur(mask, mask, Size(3, 3), 1.0);
    Mat mask_f;
    mask.convertTo(mask_f, CV_32F); // 归一化到 0~1 之间，转为float
    normalize(mask_f, mask_f, 1, 0, NORM_MINMAX);

    int rows = src.rows;
    int cols = src.cols;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            for (int c = 0; c < 3; ++c) {
//                __android_log_print(ANDROID_LOG_ERROR, "alpha", "%d", blur_mat.at<Vec4b>(row, col)[3]);
                float source = src.at<Vec4b>(row, col)[c];
                float blur = blur_mat.at<Vec4b>(row, col)[c];
                float k = mask_f.at<float>(row, col);
                dst.at<Vec4b>(row, col)[c] = k * blur + (1 - k) * source;
            }
            dst.at<Vec4b>(row, col)[3] = 255;   // alpha 通道
        }
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_meanBlurByInteg(JNIEnv *env, jobject thiz,
                                                                      jobject src) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);
    int size = 17;
    Mat dst;
    fastBilateralBlur(mat, dst, size, size * size);
    Mat skinMask;
    skinDetect(mat, skinMask);
    Mat fuseDst;
    skinFuse(mat, dst, fuseDst, skinMask);

    // 边缘增强
    Mat cannyMask;
    Canny(mat, cannyMask, 150, 300, 3, false);
    // 叠加
    bitwise_and(mat, mat, fuseDst, cannyMask);
    // 提高亮度
    add(fuseDst, Scalar(10, 10, 10, 0), fuseDst);

    cv_helper::mat2bitmap(env, fuseDst, src);
    return 1;
}