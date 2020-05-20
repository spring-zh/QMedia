//
// Created by spring on 2020/5/19.
//

#include <Utils/Comm.h>
#include "VideoTargetAdapter.h"
#include "j4a_generate/j4a_generate.h"
#include "MediaDescribe_jni.h"

VideoTargetAdapter::VideoTargetAdapter(jobject jtarget) {
    JNIEnv* env = JniUtils::getEnv();
    _object = env->NewWeakGlobalRef(jtarget);
    static CallOnce once(&J4A_loadClass__J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget, env);
}
VideoTargetAdapter::~VideoTargetAdapter() {
    JNIEnv* env = JniUtils::getEnv();
    env->DeleteWeakGlobalRef(_object);
}

bool VideoTargetAdapter::start() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__startVideo(env, _object);
}

bool VideoTargetAdapter::stop() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__stopVideo(env, _object);
}

void VideoTargetAdapter::pause(bool isPause) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__pauseVideo(env, _object, isPause);
}

void VideoTargetAdapter::flush() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__flushVideo(env, _object);
}

bool VideoTargetAdapter::init(VideoDescribe videoDesc) {
    JNIEnv* env = JniUtils::getEnv();
    //TODO: VideoDescribe --> QVideoDescribe
    jobject jvideoDesc = MediaDescribe_jni::VideoDescribeToJava(env, videoDesc);
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__initVideo(env, _object, jvideoDesc);
}

const int VideoTargetAdapter::getWidth() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetWidth(env, _object);
}

const int VideoTargetAdapter::getHeight() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetHeight(env, _object);
}

const float VideoTargetAdapter::getFrameRate() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getFrameRate(env, _object);
}

void * VideoTargetAdapter::getRenderContext() const {
    JNIEnv* env = JniUtils::getEnv();
    jobject jcontext = J4AC_com_qmedia_qmediasdk_QTarget_QVideoTarget__getTargetContext(env, _object);
    return (void*)jcontext;
}
