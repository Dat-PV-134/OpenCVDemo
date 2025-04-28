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



