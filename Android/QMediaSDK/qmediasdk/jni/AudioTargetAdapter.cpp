//
// Created by spring on 2020/5/19.
//

#include "Utils/Comm.h"
#include "AudioTargetAdapter.h"
#include "j4a_generate/j4a_generate.h"
#include "MediaDescribe_jni.h"

AudioTargetAdapter::AudioTargetAdapter(jobject jtarget) {
    JNIEnv* env = JniUtils::getEnv();
    _object = env->NewWeakGlobalRef(jtarget);
    static CallOnce once(&J4A_loadClass__J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget, env);
}
AudioTargetAdapter::~AudioTargetAdapter() {
    JNIEnv* env = JniUtils::getEnv();
    env->DeleteWeakGlobalRef(_object);
}

bool AudioTargetAdapter::init(AudioDescribe audioDesc) {
    JNIEnv* env = JniUtils::getEnv();
    //TODO: AudioDescribe --> QAudioDescribe
    jobject jaudioDesc = MediaDescribe_jni::AudioDescribeToJava(env, audioDesc);
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__initAudio(env, _object, jaudioDesc);
}

void AudioTargetAdapter::unInit() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__unInitAudio(env, _object);
}

int AudioTargetAdapter::getSampleRate() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getSampleRate(env, _object);
}

int AudioTargetAdapter::getChannels() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getChannels(env, _object);
}

RawAudioFormat AudioTargetAdapter::getFormat() const {
    JNIEnv* env = JniUtils::getEnv();
    return (RawAudioFormat)J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getFormat(env, _object);
}

void AudioTargetAdapter::setVolume(float volume) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__setVolume(env, _object, volume);
}

float AudioTargetAdapter::getVolume() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getVolume(env, _object);
}

int AudioTargetAdapter::getAudioDelayBytes() const {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__getAudioDelay(env, _object);
}

bool AudioTargetAdapter::start() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__startAudio(env, _object);
}

bool AudioTargetAdapter::stop() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__stopAudio(env, _object);
}

void AudioTargetAdapter::pause(bool isPause) {
    JNIEnv* env = JniUtils::getEnv();
    if (isPause)
        J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__pauseAudio(env, _object);
    else
        J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__resumeAudio(env, _object);
}

void AudioTargetAdapter::flush() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QTarget_QAudioTarget__flushAudio(env, _object);
}
