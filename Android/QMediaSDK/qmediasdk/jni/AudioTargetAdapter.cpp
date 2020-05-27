//
// Created by spring on 2020/5/19.
//

#include "Utils/Comm.h"
#include "AudioTargetAdapter.h"
#include "j4a_generate/j4a_generate.h"
#include "QMediaDescribe_Jni.h"

AudioTargetAdapter::AudioTargetAdapter(jobject jtarget):_weakObj(jtarget) {
}
AudioTargetAdapter::~AudioTargetAdapter() {
}

bool AudioTargetAdapter::init(AudioDescribe audioDesc) {
    JNIEnv* env = JniUtils::getEnv();
    //TODO: AudioDescribe --> QAudioDescribe
    jobject jaudioDesc = QMediaDescribe_Jni::AudioDescribeToJava(env, audioDesc);
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__initAudio(env, _weakObj, jaudioDesc);
}

void AudioTargetAdapter::unInit() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__unInitAudio(env, _weakObj);
}

int AudioTargetAdapter::getSampleRate() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getSampleRate(env, _weakObj);
}

int AudioTargetAdapter::getChannels() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getChannels(env, _weakObj);
}

RawAudioFormat AudioTargetAdapter::getFormat() const {
    JNIEnv* env = JniUtils::getEnv();
    return (RawAudioFormat)J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getFormat(env, _weakObj);
}

void AudioTargetAdapter::setVolume(float volume) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__setVolume(env, _weakObj, volume);
}

float AudioTargetAdapter::getVolume() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getVolume(env, _weakObj);
}

int AudioTargetAdapter::getAudioDelayBytes() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getAudioDelay(env, _weakObj);
}

bool AudioTargetAdapter::start() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__startAudio(env, _weakObj);
}

bool AudioTargetAdapter::stop() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__stopAudio(env, _weakObj);
}

void AudioTargetAdapter::pause(bool isPause) {
    JNIEnv* env = JniUtils::getEnv();
    if (isPause)
        J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__pauseAudio(env, _weakObj);
    else
        J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__resumeAudio(env, _weakObj);
}

void AudioTargetAdapter::flush() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__flushAudio(env, _weakObj);
}
