//
// Created by spring on 2020/5/19.
//

#include "MediaSourceAdapter.h"
#include "j4a_generate/j4a_generate.h"

MediaSourceAdapter::MediaSourceAdapter(jobject jsource)
{
    JNIEnv* env = JniUtils::getEnv();
    _object = env->NewWeakGlobalRef(jsource);
    //make sure jni field and method only call once
    static CallOnce once(&J4A_loadClass__J4AC_com_qmedia_qmediasdk_QSource_QMediaSource, env);
}
MediaSourceAdapter::~MediaSourceAdapter()
{
    JNIEnv* env = JniUtils::getEnv();
    env->DeleteWeakGlobalRef(_object);
}

bool MediaSourceAdapter::load() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__load(env, _object);
}

void MediaSourceAdapter::unload() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__unload(env, _object);
}

bool MediaSourceAdapter::isInit() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__isInit(env, _object);
}

void MediaSourceAdapter::stop() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__stop(env, _object);
}

bool MediaSourceAdapter::seekTo(int64_t mSec) {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__seekTo(env, _object, mSec);
}

bool MediaSourceAdapter::isEOF() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__isEOF(env, _object);
}

VideoFrameDrawer *MediaSourceAdapter::createVideoFrameDrawer() {
    return nullptr;
}

VideoFrame MediaSourceAdapter::readNextVideoFrame(int &error) {
    JNIEnv* env = JniUtils::getEnv();
    jobject jframe = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__readNextVideoFrame(env, _object);
    return VideoFrame(nullptr, 0);
}

AudioFrame MediaSourceAdapter::readNextAudioFrame(int &error) {
    JNIEnv* env = JniUtils::getEnv();
    jobject jframe = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__readNextAudioFrame(env, _object);
    return AudioFrame(nullptr, 0, 0);
}
