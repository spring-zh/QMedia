//
// Created by spring on 2020/5/19.
//

#include "QMediaDescribe_Jni.h"
#include "j4a_generate/j4a_generate.h"
#include <mutex>
#include <Utils/Comm.h>

jobject QMediaDescribe_Jni::VideoDescribeToJava(JNIEnv* env, VideoDescribe videoDescribe) {

    return J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__QVideoDescribe__catchAll(env,
            (jint)videoDescribe.codectype,
            (jint)videoDescribe.format,
            videoDescribe.rotation,
            videoDescribe.framerate,
            videoDescribe.width, videoDescribe.height,
            videoDescribe.bitrate, videoDescribe.timeScale);
}
jobject QMediaDescribe_Jni::AudioDescribeToJava(JNIEnv* env, AudioDescribe audioDescribe) {

    return J4AC_com_qmedia_qmediasdk_QSource_QAudioDescribe__QAudioDescribe__catchAll(env, (jint)audioDescribe.codectype,
            (jint)audioDescribe.format,
            audioDescribe.samplerate,
            audioDescribe.nchannel,
            audioDescribe.bitrate);
}

VideoDescribe QMediaDescribe_Jni::JavaToVideoDescribe(JNIEnv* env, jobject jvideoDescribe) {
    VideoDescribe videoDescribe;
    videoDescribe.codectype = (VideoCodecType)J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__codec__get(env, jvideoDescribe);
    videoDescribe.format = (RawVideoFormat)J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rawFormat__get(env, jvideoDescribe);
    videoDescribe.rotation = (VideoRotation)J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__rotation__get(env, jvideoDescribe);
    videoDescribe.framerate = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__framerate__get(env, jvideoDescribe);
    videoDescribe.width = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__width__get(env, jvideoDescribe);
    videoDescribe.height = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__height__get(env, jvideoDescribe);
    videoDescribe.bitrate = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__bitrate__get(env, jvideoDescribe);
    videoDescribe.timeScale = J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe__timeScale__get(env, jvideoDescribe);
    return videoDescribe;
}
AudioDescribe QMediaDescribe_Jni::JavaToAudioDescribe(JNIEnv* env, jobject jaudioDescribe) {
    AudioDescribe audioDescribe;
    audioDescribe.codectype = (AudioCodecType)J4AC_com_qmedia_qmediasdk_QSource_QAudioDescribe__codec__get(env, jaudioDescribe);
    audioDescribe.format = (RawAudioFormat)J4AC_com_qmedia_qmediasdk_QSource_QAudioDescribe__rawFormat__get(env, jaudioDescribe);
    audioDescribe.samplerate = J4AC_com_qmedia_qmediasdk_QSource_QAudioDescribe__samplerate__get(env, jaudioDescribe);
    audioDescribe.nchannel = J4AC_com_qmedia_qmediasdk_QSource_QAudioDescribe__nchannel__get(env, jaudioDescribe);
    audioDescribe.bitrate = J4AC_com_qmedia_qmediasdk_QSource_QAudioDescribe__bitrate__get(env, jaudioDescribe);
    return audioDescribe;
}