//
// Created by spring on 2020/5/19.
//

#include <Utils/Comm.h>
#include "VideoTargetAdapter.h"
#include "j4a_generate/j4a_generate.h"
#include "QMediaDescribe_Jni.h"

VideoTargetAdapter::VideoTargetAdapter(jobject jtarget):_weakObj(jtarget) {
}
VideoTargetAdapter::~VideoTargetAdapter() {
}

bool VideoTargetAdapter::start() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__startVideo(env, _weakObj);
}

bool VideoTargetAdapter::stop() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__stopVideo(env, _weakObj);
}

void VideoTargetAdapter::pause(bool isPause) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__pauseVideo(env, _weakObj, isPause);
}

void VideoTargetAdapter::flush() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__flushVideo(env, _weakObj);
}

bool VideoTargetAdapter::init(VideoDescribe videoDesc) {
    JNIEnv* env = JniUtils::getEnv();
    //TODO: VideoDescribe --> QVideoDescribe
    jobject jvideoDesc = QMediaDescribe_Jni::VideoDescribeToJava(env, videoDesc);
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__initVideo(env, _weakObj, jvideoDesc);
}

const int VideoTargetAdapter::getWidth() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetWidth(env, _weakObj);
}

const int VideoTargetAdapter::getHeight() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetHeight(env, _weakObj);
}

const float VideoTargetAdapter::getFrameRate() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getFrameRate(env, _weakObj);
}

void * VideoTargetAdapter::getRenderContext() const {
    JNIEnv* env = JniUtils::getEnv();
    jobject jcontext = J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetContext(env, _weakObj);
    return (void*)jcontext;
}
