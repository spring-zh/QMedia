//
// Created by spring on 2020/5/19.
//

#include "MediaSourceAdapter.h"
#include "QMediaDescribe_Jni.h"
#include "QVideoFrame_Jni.h"
#include "QAudioFrame_Jni.h"
#include "j4a_generate/j4a_generate.h"

MediaSourceAdapter::MediaSourceAdapter(jobject jsource): _weakObj(jsource){
}
MediaSourceAdapter::~MediaSourceAdapter(){
}

bool MediaSourceAdapter::load() {
    JNIEnv* env = JniUtils::getEnv();
    if (J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__isInit(env, _weakObj))
        return true;
    if(J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__load(env, _weakObj)){
        _media_duration = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__getMediaDuration(env, _weakObj);
        jobject jvideoDesc = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__getVideoDescribe(env, _weakObj);
        if (jvideoDesc) {
            MediaDescribe mediaDescribe;
            mediaDescribe.mediatype_ = MediaType::Video;
            mediaDescribe._videodesc =  QMediaDescribe_Jni::JavaToVideoDescribe(env , jvideoDesc);
            _mediaDescs.push_back(mediaDescribe);
        }
        jobject jaudioDesc = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__getAudioDescribe(env, _weakObj);
        if (jaudioDesc) {
            MediaDescribe mediaDescribe;
            mediaDescribe.mediatype_ = MediaType::Audio;
            mediaDescribe._audiodesc = QMediaDescribe_Jni::JavaToAudioDescribe(env, jaudioDesc);
            _mediaDescs.push_back(mediaDescribe);
        }
        _videoTargetAdapter = std::unique_ptr<VideoTargetAdapter>(new VideoTargetAdapter(J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__getVideoTarget(env, _weakObj)));
        _audioTargetAdapter = std::unique_ptr<AudioTargetAdapter>(new AudioTargetAdapter(J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__getAudioTarget(env, _weakObj)));
        MediaSource::setVideoTarget(_videoTargetAdapter.get());
        MediaSource::setAudioTarget(_audioTargetAdapter.get());
        return true;
    }
    return false;
}

void MediaSourceAdapter::unload() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__unload(env, _weakObj);
    _mediaDescs.clear();
}

bool MediaSourceAdapter::isInit() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__isInit(env, _weakObj);
}

bool MediaSourceAdapter::start(int64_t startMSec, int64_t endMSec)
{
    JNIEnv* env = JniUtils::getEnv();
    return (_isStarted = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__start(env,_weakObj, startMSec, endMSec));
}
void MediaSourceAdapter::stop() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__stop(env, _weakObj);
    _isStarted = false;
}

bool MediaSourceAdapter::seekTo(int64_t mSec) {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__seekTo(env, _weakObj, mSec);
}

bool MediaSourceAdapter::isEOF() {
    JNIEnv* env = JniUtils::getEnv();
    return J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__isEOF(env, _weakObj);
}

VideoFrameDrawer *MediaSourceAdapter::createVideoFrameDrawer() {
    return new OESVideoFrameDrawer(_videoTarget);
}

VideoFrame MediaSourceAdapter::readNextVideoFrame(int &error) {
    JNIEnv* env = JniUtils::getEnv();
    error = 1;
    jobject jframe = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__readNextVideoFrame(env, _weakObj);
    VideoFrame videoFrame(nullptr, 0);
    if (jframe) {
        videoFrame = createVideoFrameByObject(env, jframe);
        env->DeleteLocalRef(jframe);
        error = 0;
    }
    return videoFrame;
}

AudioFrame MediaSourceAdapter::readNextAudioFrame(int &error) {
    JNIEnv* env = JniUtils::getEnv();
    error = 1;
    jobject jframe = J4AC_com_qmedia_qmediasdk_QSource_QMediaSource__readNextAudioFrame(env, _weakObj);
    AudioFrame audioFrame(nullptr, 0, 0);
    if (jframe) {
        audioFrame = createAudioFrameByObject(env, jframe);
        env->DeleteLocalRef(jframe);
        error = 0;
    }
    return audioFrame;
}
