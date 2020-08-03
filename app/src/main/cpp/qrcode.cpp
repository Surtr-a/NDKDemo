//
// Created by 30399 on 2020/7/8.
//

#include <jni.h>
#include "opencv2/opencv.hpp"
#include "cv_helper.h"
#include <android/log.h>
#include <vector>

using namespace cv;
using namespace std;

Mat warpTransform(const Mat &gray, const RotatedRect &rect) {
    int width = rect.size.width;
    int height = rect.size.height;
    Mat result(Size(width, height), gray.type());

    vector<Point> srcPoints;
    Point2f pts[4];
    rect.points(pts);   // 获取四个顶点
    for (auto & pt : pts) {
        srcPoints.emplace_back(pt);
    }
    vector<Point> dstPoints;
    dstPoints.emplace_back(0, 0);
    dstPoints.emplace_back(width - 1, 0);
    dstPoints.emplace_back(width - 1, height - 1);
    dstPoints.emplace_back(0, height - 1);

    // 单映射，找到两个平面之间的转换矩阵
    Mat M = findHomography(srcPoints, dstPoints);
    // 透视变换
    warpPerspective(gray, result, M, result.size());

    return result;
}

bool isXCorner(const Mat &qrROI) {
    // 中间白色块宽度，左边白色块宽度，右边白色块宽度，左边黑色块宽度，右边黑色块宽度
    int cb = 0, lw = 0, rw = 0, lb = 0, rb = 0;

    int width = qrROI.cols;
    int height = qrROI.rows;
    int cx = width / 2;
    int cy = height / 2;
    uchar pixel = qrROI.at<uchar>(cy, cx);
    // 轮廓查找出来会在四周边缘多加1像素
    // 如果二维码的三个色块比较大，则先筛选掉中心和四个角不是黑色的色块，否则只需要判断中间的像素颜色
//    if (width >= 23) {  // 7 * 3 + 2
//        uchar p1 = qrROI.at<uchar>(2, 2);
//        uchar p2 = qrROI.at<uchar>(2, width - 3);
//        uchar p3 = qrROI.at<uchar>(height - 3, 2);
//        uchar p4 = qrROI.at<uchar>(height - 3, width - 3);
//        if (pixel == 255 || p1 == 255 || p2 == 255 || p3 == 255 || p4 == 255) {
//            return false;
//        }
//    } else {
//        if (pixel == 255) {
//            return false;
//        }
//    }
    if (pixel == 255) {
        return false;
    }

    // 从中间向两边扫，获取中间黑色块的宽度
    int start = 0, end = 0, offset = 0;
    bool isFindLeft = false, isFindRight = false;
    while (true) {
        ++offset;

        if ((cx - offset) <= 0 || (cx + offset) >= width - 1) { // 扫完，两边至少有两个像素
            break;
        }

        // 向左
        pixel = qrROI.at<uchar>(cy, cx - offset);
        if (!isFindLeft && pixel == 255) {
            start = cx - offset;
            isFindLeft = true;
        }

        // 向右
        pixel = qrROI.at<uchar>(cy, cx + offset);
        if (!isFindRight && pixel == 255) {
            end = cx + offset;
            isFindRight = true;
        }

        if (isFindLeft && isFindRight) {
            break;
        }
    }
    if (start == 0 || end == 0) {
        return false;
    }
    cb = end - start;   // 中间黑色块宽度

    // 相间的白色区域
    for (int col = end; col < width - 1; ++col) {   // 第一个像素和最后一个像素一定是黑色
        pixel = qrROI.at<uchar>(cy, col);
        if (pixel == 0) {
            break;
        }
        ++rw;
    }
    for (int col = start; col > 0; --col) {
        pixel = qrROI.at<uchar>(cy, col);
        if (pixel == 0) {
            break;
        }
        ++lw;
    }
    if (rw == 0 || lw == 0) {
        return false;
    }

    // 两边的黑色
    for (int col = end + rw; col < width; ++col) {
        pixel = qrROI.at<uchar>(cy, col);
        if (pixel == 255) {
            break;
        }
        ++rb;
    }
    for (int col = start - lw; col >= 0; --col) {
        pixel = qrROI.at<uchar>(cy, col);
        if (pixel == 255) {
            break;
        }
        ++lb;
    }
    if (rb == 0 || lb == 0) {
        return false;
    }

    // 求比例
    float sum = cb + lb + rb + lw + rw;
    __android_log_print(ANDROID_LOG_ERROR, "rrrrrrrrrr", "size:%lf", sum);
    cb = (cb / sum) * 7.0 + 0.5;
    lb = (lb / sum) * 7.0 + 0.5;
    rb = (rb / sum) * 7.0 + 0.5;
    lw = (lw / sum) * 7.0 + 0.5;
    rw = (rw / sum) * 7.0 + 0.5;
    // 取整后为 3:1:1:1:1 或 4:1:1:1:1
    return (cb == 3 || cb == 4) && lw == rw && lb == rb && lw == rb && lw == 1;
}

// 减少误差，对Y方向再进行判断，但可以进行简化
bool isYCorner(const Mat& qrROI) {
    // 统计白色像素点和黑色像素点
    int bp = 0, wp = 0;
    int width = qrROI.cols;
    int height = qrROI.rows;
    int cx = width / 2;

    int pixel = 0;
    for (int row = 0; row < height; ++row) {
        pixel = qrROI.at<uchar>(row, cx);
        if (pixel == 0) {
            ++bp;
        } else if (pixel == 255) {
            ++wp;
        }
    }

    if (bp == 0 || wp == 0) {
        return false;
    }

    // 黑白像素比在 5:2 左右
    return !(wp * 2 > bp && bp > 3 * wp);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_getQRCodeArea(JNIEnv *env, jobject thiz,
                                                                    jobject src) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);
    Mat gray;
    cvtColor(mat, gray, COLOR_BGRA2GRAY);
    // 二值化，自动阈值
    threshold(gray, gray, 0, 255, THRESH_BINARY | THRESH_OTSU);
    // 轮廓查找
    vector<vector<Point>> contours;
    findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    __android_log_print(ANDROID_LOG_ERROR, "rrrrrrrrrr", "contours num%lu", contours.size());

    for (int i = 0; i < contours.size(); ++i) {
        // 根据面积过滤
        double area = contourArea(contours[i]);
        if (area < 49) {    // 三个色块最小像素面积
            continue;
        }

        // 根据宽高比和大小过滤
        RotatedRect rRect = minAreaRect(contours[i]);   // 计算包含轮廓的最小矩形，会自动旋转
        float w = rRect.size.width;
        float h = rRect.size.height;
        float ratio = min(w, h) / max(w, h);
        if (ratio > 0.9 && w < gray.cols / 2 && h < gray.rows / 2) {
            // 扭曲变换，将三个正方形摆正
            Mat qrROI = warpTransform(gray, rRect);

            // 检测，先对相对简单的Y方向检测
            if (isYCorner(qrROI) && isXCorner(qrROI)) {
                drawContours(mat, contours, i, Scalar(255, 0, 0, 255), 4);
            }
        }
    }

    cv_helper::mat2bitmap(env, gray, src);
    return 1;
}