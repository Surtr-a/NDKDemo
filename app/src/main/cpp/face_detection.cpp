//
// Created by 30399 on 2020/7/11.
//

#include <jni.h>
#include <string>
#include "opencv2/opencv.hpp"
#include <android/log.h>
#include "opencv2/face.hpp"

using namespace cv;
using namespace face;
using namespace std;

#define TAG "FACE_TAG"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

CascadeClassifier cascadeClassifier;
Ptr<BasicFaceRecognizer> model = EigenFaceRecognizer::create();

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_FaceDetection_loadCascade(JNIEnv *env, jobject thiz,
                                                           jstring file_path) {
    const char *filePath = env->GetStringUTFChars(file_path, 0);

    cascadeClassifier.load(filePath);
    LOGE("人脸识别级联分类器加载成功");
    env->ReleaseStringChars(file_path, reinterpret_cast<const jchar *>(filePath));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_FaceDetection_faceDetection(JNIEnv *env, jobject thiz,
                                                             jlong native_obj) {
    Mat *src = reinterpret_cast<Mat *>(native_obj);

    int width = src->rows;
    int height = src->cols;

    // 转成灰度图，提升运算速度
    Mat grayMat;
    cvtColor(*src, grayMat, COLOR_BGRA2GRAY);

    // 检测人脸
    // faces - 检测出的人脸
    // 1.1 - 缩放比例，会采取上采样和降采样
    // 3 - 检测次数，根据前一个参数缩放后再检测，3即三次检测的大小为 size，1.1*size，1.1*1.1*size
    // Size(width / 2, height / 2) - 人脸最小大小
    std::vector<Rect> faces;
    cascadeClassifier.detectMultiScale(grayMat, faces, 1.1, 3, 0, Size(width / 4, height / 4));
    LOGE("人脸size = %lu", faces.size());

    if (faces.size() != 1) {
        return;
    }
    // 框出来
    Rect faceRect = faces[0];
    rectangle(*src, faceRect, Scalar(255, 0, 0, 255), 4, LINE_AA);

    // 与服务端/本地进行对比
    Mat face = (*src)(faceRect).clone();
    resize(face, face, Size(128, 128));
    cvtColor(face, face, COLOR_BGRA2GRAY);
    int label = model->predict(face);
    if (label == 11) {  // 训练时对应的label
        LOGE("识别到了自己");
        putText(*src, "Jiuxs", Point(faceRect.x + 20, faceRect.y + 20),
                HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0, 255), 2, LINE_AA);
    } else {
        LOGE("不是自己");
        putText(*src, "Unknown", Point(faceRect.x + 20, faceRect.y + 20),
                HersheyFonts::FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0, 255), 2, LINE_AA);
    }
}


// 训练样本
extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_FaceDetection_trainingPattern(JNIEnv *env, jobject thiz) {
    vector<Mat> faces;
    vector<int> labels;

    for (int i = 1; i <= 10; ++i) {
        for (int j = 1; j <= 10; ++j) {
            Mat face = imread(format("/storage/emulated/0/facedetection/orl/s%d/%d.pgm", i, j), 0);
            if (face.empty()) {
                LOGE("face mat is empty");
                continue;
            }
            resize(face, face, Size(128, 128));
            faces.emplace_back(face);
            labels.emplace_back(i);
        }
    }

    for (int i = 1; i < 8; ++i) {
        Mat face = imread(format("/storage/emulated/0/facedetection/my/face_%d.jpg", i), 0);
        if (face.empty()) {
            LOGE("face mat is empty");
            continue;
        }
        resize(face, face, Size(128, 128));
        faces.emplace_back(face);
        labels.emplace_back(11);
    }

    // 训练方法
    // Ptr<BasicFaceRecognizer> model = EigenFaceRecognizer::create();
    // 采集了8张自己的人脸，同一个人label一样
    model->train(faces, labels);
    // 存储训练样本（特征数据）
    model->save("/storage/emulated/0/facedetection/face_my_pattern.xml");
    LOGE("样本训练成功");
}

// 加载样本数据
extern "C"
JNIEXPORT void JNICALL
Java_com_flag_demo_ndkdemo_utils_FaceDetection_loadPattern(JNIEnv *env, jobject thiz,
                                                           jstring pattern_path) {
    const char *patternPath = env->GetStringUTFChars(pattern_path, 0);
    // 加载样本数据
    // model->load(patternPath);    Error，直接拷贝方法实现
    FileStorage fs(patternPath, FileStorage::READ);
    FileNode fn = fs.getFirstTopLevelNode();
    model->read(fn);
    env->ReleaseStringUTFChars(pattern_path, patternPath);
    LOGE("训练样本加载成功");
}