#include <jni.h>
#include <string>
#include <android/bitmap.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <android/log.h>

using namespace cv;

#define TAG "NativeLib"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_rekoj134_nativelib_NativeLib_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_rekoj134_nativelib_NativeLib_gray(
        JNIEnv *env, jobject /* this */,
        jobject srcBitmap, jobject dstBitmap) {
    AndroidBitmapInfo info;
    void* srcPixels;
    void* dstPixels;

    // Lấy thông tin bitmap
    if (AndroidBitmap_getInfo(env, srcBitmap, &info) < 0) return;
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) return;

    // Lock pixels
    if (AndroidBitmap_lockPixels(env, srcBitmap, &srcPixels) < 0) return;
    if (AndroidBitmap_lockPixels(env, dstBitmap, &dstPixels) < 0) {
        AndroidBitmap_unlockPixels(env, srcBitmap);
        return;
    }

    // Tạo Mat từ src
    Mat srcMat(info.height, info.width, CV_8UC4, srcPixels);
    Mat grayMat;
    cvtColor(srcMat, grayMat, COLOR_RGBA2GRAY);

    // Chuyển về lại RGBA để hiển thị đúng trên Android
    Mat dstMat(info.height, info.width, CV_8UC4, dstPixels);
    cvtColor(grayMat, dstMat, COLOR_GRAY2RGBA);

    // Unlock pixels
    AndroidBitmap_unlockPixels(env, srcBitmap);
    AndroidBitmap_unlockPixels(env, dstBitmap);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_rekoj134_nativelib_NativeLib_test(
        JNIEnv *env, jobject /* this */,
        jobject srcBitmap, jobject dstBitmap) {
    AndroidBitmapInfo info;
    void* srcPixels;
    void* dstPixels;

    // Lấy thông tin bitmap
    if (AndroidBitmap_getInfo(env, srcBitmap, &info) < 0) return;
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) return;

    // Lock pixels
    if (AndroidBitmap_lockPixels(env, srcBitmap, &srcPixels) < 0) return;
    if (AndroidBitmap_lockPixels(env, dstBitmap, &dstPixels) < 0) {
        AndroidBitmap_unlockPixels(env, srcBitmap);
        return;
    }

    // Tạo Mat từ src
    Mat srcMat(info.height, info.width, CV_8UC4, srcPixels);
    Mat grayMat;
    cvtColor(srcMat, grayMat, COLOR_RGBA2GRAY);

    // Blur
    Mat blurMat;
    GaussianBlur(grayMat, blurMat, Size(111, 111), 0);

    // Chuyển về lại RGBA để hiển thị đúng trên Android
    Mat dstMat(blurMat.size(), CV_8UC4, dstPixels);
    cvtColor(blurMat, dstMat, COLOR_GRAY2RGBA);

    // Unlock pixels
    AndroidBitmap_unlockPixels(env, srcBitmap);
    AndroidBitmap_unlockPixels(env, dstBitmap);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_rekoj134_nativelib_NativeLib_enhanceImage(
        JNIEnv *env, jobject /* this */, jobject srcBitmap, jobject dstBitmap) {

    AndroidBitmapInfo info;
    void* srcPixels;
    void* dstPixels;

    if (AndroidBitmap_getInfo(env, srcBitmap, &info) < 0) return;
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) return;

    if (AndroidBitmap_lockPixels(env, srcBitmap, &srcPixels) < 0) return;
    if (AndroidBitmap_lockPixels(env, dstBitmap, &dstPixels) < 0) {
        AndroidBitmap_unlockPixels(env, srcBitmap);
        return;
    }

    // Tạo Mat từ src
    Mat srcMat(info.height, info.width, CV_8UC4, srcPixels);
    Mat srcRGB;
    cvtColor(srcMat, srcRGB, COLOR_RGBA2RGB);

    // 1. Resize bằng Lanczos
    Mat img_up;
    resize(srcRGB, img_up, Size(), 2.0, 2.0, INTER_LANCZOS4);

    // 2. Tăng tương phản bằng CLAHE
    Mat lab;
    cvtColor(img_up, lab, COLOR_RGB2Lab);
    std::vector<Mat> lab_planes(3);
    split(lab, lab_planes);

    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    clahe->apply(lab_planes[0], lab_planes[0]);

    merge(lab_planes, lab);
    Mat img_contrast;
    cvtColor(lab, img_contrast, COLOR_Lab2RGB);

    // 3. Làm sắc nét
    Mat kernel = (Mat_<float>(3, 3) <<
                                    0, -1, 0,
            -1, 5, -1,
            0, -1, 0);
    Mat img_sharp;
    filter2D(img_contrast, img_sharp, -1, kernel);

    // Resize về kích thước gốc nếu cần giữ kích thước đầu vào
    Mat finalResult;
    resize(img_sharp, finalResult, Size(info.width, info.height), 0, 0, INTER_LANCZOS4);

    // Chuyển về RGBA để gán vào bitmap output
    Mat dstMat(info.height, info.width, CV_8UC4, dstPixels);
    cvtColor(finalResult, dstMat, COLOR_RGB2RGBA);

    AndroidBitmap_unlockPixels(env, srcBitmap);
    AndroidBitmap_unlockPixels(env, dstBitmap);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_rekoj134_nativelib_NativeLib_resizeImage(
        JNIEnv *env, jobject /* this */, jobject srcBitmap, jobject dstBitmap, jint targetWidth, jint targetHeight) {

    AndroidBitmapInfo info;
    void* srcPixels;
    void* dstPixels;

    // Lấy thông tin ảnh nguồn
    if (AndroidBitmap_getInfo(env, srcBitmap, &info) < 0) return;
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) return;

    // Lock các pixel của ảnh nguồn và ảnh đích
    if (AndroidBitmap_lockPixels(env, srcBitmap, &srcPixels) < 0) return;
    if (AndroidBitmap_lockPixels(env, dstBitmap, &dstPixels) < 0) {
        AndroidBitmap_unlockPixels(env, srcBitmap);
        return;
    }

    // Tạo Mat từ bitmap nguồn
    Mat srcMat(info.height, info.width, CV_8UC4, srcPixels);

    // Resize ảnh
    Mat resizedMat;
    Size targetSize(targetWidth, targetHeight);
    resize(srcMat, resizedMat, targetSize, 0, 0, INTER_AREA);

    // Chuyển ảnh đã resize về lại bitmap đích
    Mat dstMat(targetHeight, targetWidth, CV_8UC4, dstPixels);
    cvtColor(resizedMat, dstMat, COLOR_RGB2RGBA);  // Chuyển lại từ RGB về RGBA

    // Unlock pixels
    AndroidBitmap_unlockPixels(env, srcBitmap);
    AndroidBitmap_unlockPixels(env, dstBitmap);
}


