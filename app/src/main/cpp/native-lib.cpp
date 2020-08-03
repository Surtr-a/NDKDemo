#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/log.h>
#include <android/bitmap.h>
#include <vector>
#include <opencv2/core/core_c.h>
#include <iostream>
#include "cv_helper.h"
#include "bitmap_util.h"

using namespace cv;
using namespace std;

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_gray3(JNIEnv *env, jobject thiz,
                                                            jobject src) {
    AndroidBitmapInfo bitmapInfo;
    int info_res = AndroidBitmap_getInfo(env, src, &bitmapInfo);

    if (info_res != 0) {    // 等于0成功获取了对象，否则返回操作失败
        return -1;
    }

    __android_log_print(ANDROID_LOG_ERROR, "TTTTTTT", "%d", bitmapInfo.format);

    void *pixels;   // void指针，并不知道具体类型
    AndroidBitmap_lockPixels(env, src, &pixels);    // 锁定

    if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        for (int i = 0; i < bitmapInfo.width * bitmapInfo.height; ++i) {
            auto *pixel_p = reinterpret_cast<uint32_t *>(pixels) + i;   // 强转
            auto pixel = *pixel_p;

            int a = (pixel >> 24) & 0xff;
            int r = (pixel >> 16) & 0xff;
            int g = (pixel >> 8) & 0xff;
            int b = pixel & 0xff;

            int gray = (int) (0.213 * r + 0.715 * g + 0.072f * b);

            *pixel_p = (a << 24) | (gray << 16) | (gray << 8) | gray;
        }
    } else if (bitmapInfo.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        for (int i = 0; i < bitmapInfo.width * bitmapInfo.height; ++i) {
            auto *pixel_p = reinterpret_cast<uint16_t *>(pixels) + i;   // 强转
            auto pixel = *pixel_p;

            // 没有α通道，并且位数不同，但之前的公式只适用于ARGB_8888，所以需要补到8位
            int r = (pixel >> 11) & 0x1f << 3;
            int g = (pixel >> 5) & 0x3f << 2;
            int b = pixel & 0x1f << 3;

            int gray = (int) (0.213 * r + 0.715 * g + 0.072f * b);

            *pixel_p = ((gray >> 3) << 11) | ((gray >> 2) << 5) | (gray >> 3);
        }
    }

    AndroidBitmap_unlockPixels(env, src);   // 解锁

//    Point pts[1][3];
//    pts[0][0] = Point(100, 100);
//    pts[0][1] = Point(100, 200);
//    pts[0][2] = Point(200, 200);
//
//    const Point *ptss[] = {pts[0]};
//    const int npts[] = {3};
//    fillPoly(mat, ptss, npts, 1, Scalar(255, 0, 0), LINE_8);
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_imageChangeFromArr(JNIEnv *env, jobject thiz,
                                                                         jobject src, jint rows,
                                                                         jint cols) {
    AndroidBitmapInfo bitmapInfo;
    int info_res = AndroidBitmap_getInfo(env, src, &bitmapInfo);

    if (info_res != 0) {    // 等于0成功获取了对象，否则返回操作失败
        return -1;
    }

    // 取得int数组
    jintArray data = env->NewIntArray(rows * cols);
    jclass j_clz = env->GetObjectClass(src);
    jmethodID j_mid = env->GetMethodID(j_clz, "getPixels", "([IIIIIII)V");
    env->CallVoidMethod(src, j_mid, data, 0, cols, 0, 0, cols, rows);
    int *pixels = env->GetIntArrayElements(data, JNI_FALSE);    // 创建Mat矩阵不能直接使用 jintArray

    // 转为Mat
    Mat mat = Mat(rows, cols, CV_8UC4, pixels);

    /*-----------------------------------------------------------------------*/
    Mat dst;
    /* 灰度处理
    cvtColor(mat, mat, COLOR_BGRA2GRAY);
    cvtColor(mat, dst, COLOR_GRAY2BGRA);
    */

    /* 形态学处理
    Mat kernel = getStructuringElement(MORPH_RECT, Size(15, 15));
    morphologyEx(mat, dst, MORPH_CLOSE, kernel);
    */

    /* Scharr边缘检测
    GaussianBlur(mat, dst, Size(3, 3), 1);
    // 转灰度
    cvtColor(dst, dst, COLOR_BGRA2GRAY);
    cvtColor(dst, dst, COLOR_GRAY2BGRA);
    // Scharr梯度增强
    Mat scharr_x, scharr_y;
    Scharr(dst, scharr_x, CV_32F, 1, 0, 3);
    Scharr(dst, scharr_y, CV_32F, 0, 1, 3);
    // 取绝对值
    convertScaleAbs(scharr_x, scharr_x);
    convertScaleAbs(scharr_y, scharr_y);
    // 图像混合
    addWeighted(scharr_x, 0.5, scharr_y, 0.5, 0, dst);
    */

    /* 霍夫直线检测
    Mat gray;
    cvtColor(mat, gray, COLOR_BGRA2GRAY);
    cvtColor(gray, gray, COLOR_GRAY2BGRA);
    cvtColor(gray, gray, COLOR_BGRA2BGR);
    Canny(gray, dst, 140, 250, 3);
    cvtColor(dst, dst,COLOR_BGR2BGRA);

    std::vector<Vec4f> pLines;
    HoughLinesP(dst, pLines, 1, CV_PI / 180, 170, 30, 0);
    for (auto pLine : pLines) {
        // 取得线
        line(mat, Point(pLine[0], pLine[1]), Point(pLine[2], pLine[3]), Scalar(0, 0, 255), 4, LINE_AA);
    }
    */

    /*
    cvtColor(mat, mat, COLOR_BGRA2BGR);
    Mat map_x(mat.size(), CV_32FC1);
    Mat map_y(mat.size(), CV_32FC1);
    // 水平翻转
    for (int row = 0; row < mat.rows; ++row) {
        for (int col = 0; col < mat.cols; ++col) {
            map_x.at<float>(row, col) = static_cast<float>(mat.cols - col);
            map_y.at<float>(row, col) = row;
        }
    }
    remap(mat, dst, map_x, map_y, INTER_LINEAR);
    cvtColor(dst, dst, COLOR_BGR2BGRA);
    */

    /*-----------------------------------------------------------------------*/

    // 将Mat中的数据重新放入数组
    jint *newData = reinterpret_cast<jint *>(mat.ptr(0));
    env->SetIntArrayRegion(data, 0, rows * cols, newData);

    // 将数组数据重新放入Bitmap
    j_mid = env->GetMethodID(j_clz, "setPixels", "([IIIIIII)V");
    env->CallVoidMethod(src, j_mid, data, 0, cols, 0, 0, cols, rows);

    env->DeleteLocalRef(data);
    env->DeleteLocalRef(j_clz);
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_getImageHistogram(JNIEnv *env, jobject thiz,
                                                                        jobject src, jint rows,
                                                                        jint cols, jobject dst,
                                                                        jobject type) {
    AndroidBitmapInfo bitmapInfo;
    int info_res = AndroidBitmap_getInfo(env, src, &bitmapInfo);

    if (info_res != 0) {    // 等于0成功获取了对象，否则返回操作失败
        return -1;
    }

    // 取得src int数组
    jintArray data = env->NewIntArray(rows * cols);
    jclass j_clz = env->GetObjectClass(src);
    jmethodID j_mid = env->GetMethodID(j_clz, "getPixels", "([IIIIIII)V");
    env->CallVoidMethod(src, j_mid, data, 0, cols, 0, 0, cols, rows);
    int *pixels = env->GetIntArrayElements(data, JNI_FALSE);    // 创建Mat矩阵不能直接使用 jintArray

    // 转为Mat
    Mat mat = Mat(rows, cols, CV_8UC4, pixels);

    std::vector<Mat> bgr_s;
    split(mat, bgr_s);

    int histSize = 256;
    float range[] = {0, 255};
    const float *ranges = {range};
    Mat hist_b, hist_g, hist_r;
    calcHist(&bgr_s[0], 1, 0, Mat(), hist_b, 1, &histSize, &ranges, true, false);
    calcHist(&bgr_s[1], 1, 0, Mat(), hist_g, 1, &histSize, &ranges, true, false);
    calcHist(&bgr_s[2], 1, 0, Mat(), hist_r, 1, &histSize, &ranges, true, false);

    int hist_h = 400;
    int hist_w = 512;
    int bin_w = hist_w / histSize;
    normalize(hist_b, hist_b, 0, hist_h, NORM_MINMAX, -1, Mat());
    normalize(hist_g, hist_g, 0, hist_h, NORM_MINMAX, -1, Mat());
    normalize(hist_r, hist_r, 0, hist_h, NORM_MINMAX, -1, Mat());

    // 画到图上
    Mat histImage(hist_h, hist_w, CV_8SC4, Scalar());
    for (int i = 0; i < histSize; ++i) {
        line(histImage,
             Point((i - 1) * bin_w, hist_h - hist_b.at<float>(i - 1)),
             Point(i * bin_w, hist_h - hist_b.at<float>(i)),
             Scalar(255, 0, 0), bin_w, LINE_AA);
        line(histImage,
             Point((i - 1) * bin_w, hist_h - hist_g.at<float>(i - 1)),
             Point(i * bin_w, hist_h - hist_g.at<float>(i)),
             Scalar(0, 255, 0), bin_w, LINE_AA);
        line(histImage,
             Point((i - 1) * bin_w, hist_h - hist_r.at<float>(i - 1)),
             Point(i * bin_w, hist_h - hist_r.at<float>(i)),
             Scalar(0, 0, 255), bin_w, LINE_AA);
    }

    // 将Mat中的数据放入数组
    jint *newData = reinterpret_cast<jint *>(histImage.ptr(0));
    jintArray dstData = env->NewIntArray(hist_h * hist_w);
    env->SetIntArrayRegion(dstData, 0, hist_h * hist_w, newData);

    // 创建dst Bitmap对象
    jclass bitmap_clz = env->FindClass("android/graphics/Bitmap");
    j_mid = env->GetStaticMethodID(bitmap_clz, "createBitmap",
                                   "(IILandroid/graphics/Bitmap/Config;)Landroid/graphics/Bitmap;");
    dst = env->CallStaticObjectMethod(bitmap_clz, j_mid, hist_w, hist_h, type);

    // 将数组数据放入dst Bitmap
    j_clz = env->GetObjectClass(dst);
    j_mid = env->GetMethodID(j_clz, "setPixels", "([IIIIIII)V");
    env->CallVoidMethod(dst, j_mid, dstData, 0, hist_w, 0, 0, hist_w, hist_h);

    // 释放内存
    env->DeleteLocalRef(data);
    env->DeleteLocalRef(j_clz);
    env->DeleteLocalRef(dstData);
    env->DeleteLocalRef(bitmap_clz);
    return 1;
}

// 逆世界
extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_againstWorld(JNIEnv *env, jobject thiz,
                                                                   jobject src) {
    // bitmap转为mat
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);

    Mat res(mat.size(), mat.type());
    int mat_w = mat.cols;
    int mat_h = mat.rows;
    int mid_h = mat_h >> 1;
    int a_h = mid_h >> 1;

    // 处理下半部分
    for (int rows = 0; rows < mid_h; ++rows) {
        for (int cols = 0; cols < mat_w; ++cols) {
            res.at<float>(rows + mid_h, cols) = mat.at<float>(rows + a_h, cols);
        }
    }

    // 处理上半部分
    for (int rows = mid_h; rows < mat_h; ++rows) {
        for (int cols = 0; cols < mat_w; ++cols) {
            res.at<float>(mat_h - rows, cols) = res.at<float>(rows, cols);
        }
    }

    cv_helper::mat2bitmap(env, res, src);

    return 1;
}

// 浮雕
extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_relief(JNIEnv *env, jobject thiz,
                                                             jobject src) {
    // bitmap转为mat
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);

    Mat res(mat.size(), mat.type());
    int mat_w = mat.cols;
    int mat_h = mat.rows;

    for (int row = 0; row < mat_h - 1; ++row) {  // -1防止越界
        for (int col = 0; col < mat_w - 1; ++col) {
            Vec4b pixel_p = mat.at<Vec4b>(row, col);  // (0, 0)像素点
            Vec4b pixel_n = mat.at<Vec4b>(row + 1, col + 1);  // (1, 1)像素点
            // bgra
            res.at<Vec4b>(row, col)[0] = saturate_cast<uchar>(pixel_p[0] - pixel_n[0] + 128);
            res.at<Vec4b>(row, col)[1] = saturate_cast<uchar>(pixel_p[1] - pixel_n[1] + 128);
            res.at<Vec4b>(row, col)[2] = saturate_cast<uchar>(pixel_p[2] - pixel_n[2] + 128);
            res.at<Vec4b>(row, col)[3] = 255;
        }
    }

    cv_helper::mat2bitmap(env, res, src);

    return 1;
}

// 马赛克
extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_mosaic(JNIEnv *env, jobject thiz,
                                                             jobject src) {
    // bitmap转为mat
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);

    int mat_w = mat.cols;
    int mat_h = mat.rows;

    // 马赛克处理范围
    int row_s = mat_h >> 2;
    int row_e = mat_h * 3 / 4;
    int col_s = mat_w >> 2;
    int col_e = mat_w * 3 / 4;
    // 处理步长
    int size = 20;

    for (int row = row_s; row < row_e; row += size) {
        for (int col = col_s; col < col_e; col += size) {
            int pixel = mat.at<int>(row, col);
            // 其他点像素值设置为第一个点的像素值
            for (int m_row = 1; m_row < size; ++m_row) {
                for (int m_col = 1; m_col < size; ++m_col) {
                    mat.at<int>(row + m_row, col + m_col) = pixel;
                }
            }
        }
    }

    cv_helper::mat2bitmap(env, mat, src);

    return 1;
}

// 毛玻璃
extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_groundGlass(JNIEnv *env, jobject thiz,
                                                                  jobject src) {
    // bitmap转为mat
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);

    int mat_w = mat.cols;
    int mat_h = mat.rows;

    int size = 20;
    RNG rng(time(nullptr));

    for (int row = 0; row < mat_h - size; ++row) {  // -size防止越界，但会使边界得不到处理
        for (int col = 0; col < mat_w - size; ++col) {
            int random = rng.uniform(0, size);
            mat.at<int>(row, col) = mat.at<int>(row + random, col + random);
        }
    }

//    cv_helper::mat2bitmap(env, mat, src);

    return 1;
}

// 油画
extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_oilPainting(JNIEnv *env, jobject thiz,
                                                                  jobject src) {
    // bitmap转为mat
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);
    Mat gray;
    cvtColor(mat, gray, COLOR_BGRA2GRAY);
    Mat res(mat.size(), mat.type());

    int mat_w = mat.cols;
    int mat_h = mat.rows;
    int size = 20;   // 核边长

    for (int row = 0; row < mat_h; ++row) {
        for (int col = 0; col < mat_w; ++col) {
            int g[20] = {0}; // 等级灰度和
            int b_g[20] = {0};
            int g_g[20] = {0};
            int r_g[20] = {0};
            for (int o_row = 0; o_row < size; ++o_row) {
                for (int o_col = 0; o_col < size; ++o_col) {
                    uchar g_num = gray.at<uchar>(row + o_row, col + o_col); // 取得当前点灰度
                    uchar index = g_num / (255 / (size - 1));    // 获取灰度值对应的等级
                    ++g[index];
                    b_g[index] += mat.at<Vec4b>(row + o_row, col + o_col)[0];
                    g_g[index] += mat.at<Vec4b>(row + o_row, col + o_col)[1];
                    r_g[index] += mat.at<Vec4b>(row + o_row, col + o_col)[2];
                }
            }
            // 找频率最高的灰度等级
            int max_index = 0;
            int max = g[0]; // 该等级包含的像素数量
            for (int i = 0; i < size; ++i) {
                if (g[max_index] < g[i]) {
                    max_index = i;
                    max = g[i];
                }
            }
            res.at<Vec4b>(row, col)[0] = b_g[max_index] / max;
            res.at<Vec4b>(row, col)[1] = g_g[max_index] / max;
            res.at<Vec4b>(row, col)[2] = r_g[max_index] / max;
            res.at<Vec4b>(row, col)[3] = 255;
        }
    }

    cv_helper::mat2bitmap(env, res, src);

    return 1;
}

// 旋转
extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_rotation(JNIEnv *env, jobject thiz,
                                                               jobject src, jobject dst) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);

    int res_w = mat.rows;
    int res_h = mat.cols;

    Mat res(res_h, res_w, mat.type());

    for (int row = 0; row < mat.rows; ++row) {
        for (int col = 0; col < mat.cols; ++col) {
            res.at<int>(col, res_w - row - 1) = mat.at<int>(row, col);
        }
    }

    cv_helper::mat2bitmap(env, res, dst);
    return 1;
}

// 仿射变换
extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_wrapAffine(JNIEnv *env, jobject thiz,
                                                                 jobject src) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);

    Mat res(mat.size(), mat.type());

    Point2f center(mat.cols / 2, mat.rows / 2);
    double angle = 45;  // 角度
    double scale = 1;   // 缩放
    Mat M = getRotationMatrix2D(center, angle, scale);

    warpAffine(mat, res, M, mat.size());

    cv_helper::mat2bitmap(env, res, src);
    return 1;
}

// 图片缩放
extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_reSize(JNIEnv *env, jobject thiz, jobject src,
                                                             jobject dst, jint width, jint height) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);
    // 输出矩阵
    Mat res(height, width, mat.type());
    // 原图宽高
    float mat_w = mat.cols;
    float mat_h = mat.rows;

    for (int row = 0; row < res.rows; ++row) {
        for (int col = 0; col < res.cols; ++col) {
            // 计算当前点在原图中的位置
            int mat_row = row * (mat_h / height);
            int mat_col = col * (mat_w / width);
            Vec4b pixel = mat.at<Vec4b>(mat_row, mat_col);
            res.at<Vec4b>(row, col) = pixel;
        }
    }

    cv_helper::mat2bitmap(env, res, dst);
    return 1;
}

// 重映射
void remap(Mat &src, Mat &res, Mat &matX, Mat &matY) {
    res.create(src.size(), src.type());
    int res_w = res.cols;
    int res_h = res.rows;
    for (int row = 0; row < res_h; ++row) {
        for (int col = 0; col < res_w; ++col) {
            int x = matX.at<int>(row, col);
            int y = matY.at<int>(row, col);
            res.at<Vec4b>(row, col) = src.at<Vec4b>(y, x);
        }
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_reMap(JNIEnv *env, jobject thiz,
                                                            jobject src) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);
    Mat res;

    Mat matX(mat.size(), mat.type());
    Mat matY(mat.size(), mat.type());

    // 水平翻转
    for (int row = 0; row < mat.rows; ++row) {
        for (int col = 0; col < mat.cols; ++col) {
            matX.at<int>(row, col) = mat.cols - col;
            matY.at<int>(row, col) = mat.rows - row;
        }
    }

    remap(mat, res, matX, matY);

    cv_helper::mat2bitmap(env, res, src);
    return 1;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_flag_demo_ndkdemo_BitmapUtils_00024Companion_gray4(JNIEnv *env, jobject thiz,
                                                            jobject src) {
    Mat mat;
    cv_helper::bitmap2mat(env, src, mat);
    Mat gray;
    cvtColor(mat, gray, COLOR_BGRA2GRAY);
    cv_helper::mat2bitmap(env, gray, src);

    return 1;
}
