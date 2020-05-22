//
// Created by spring on 2020/5/19.
//

#include "MediaSourceAdapter.h"
#include "QVideoFrame_Jni.h"
#include "QAudioFrame_Jni.h"
#include "j4a_generate/j4a_generate.h"

MediaSourceAdapter::MediaSourceAdapter(jobject jsource)
{
    JNIEnv* env = JniUtils::getEnv();
    _object = env->NewWeakGlobalRef(jsource);
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
    return new OESVideoFrameDrawer(_videoTarget);
}

VideoFrame MediaSourceAdapter::readNextVideoFrame(int &error) {
    JNIEnv* env = JniUtils::getEnv();
    jobject jframe = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__readNextVideoFrame(env, _object);
    VideoFrame videoFrame(nullptr, 0);
    if (jframe) {
        videoFrame = createVideoFrameByObject(env, jframe);
    }
    return videoFrame;
}

AudioFrame MediaSourceAdapter::readNextAudioFrame(int &error) {
    JNIEnv* env = JniUtils::getEnv();
    jobject jframe = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__readNextAudioFrame(env, _object);
    AudioFrame audioFrame(nullptr, 0, 0);
    if (jframe) {
        audioFrame = createAudioFrameByObject(env, jframe);
    }
    return audioFrame;
}
