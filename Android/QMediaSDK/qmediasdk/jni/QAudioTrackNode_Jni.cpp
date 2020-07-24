//
// Created by spring on 2020/5/21.
//

#include "EffectEditor/MediaAudioChannel.h"
#include "EffectEditor/MediaTrackImpl.h"
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QAudio_QAudioTrackNode_##name

NATIVE_FUNCTION(jlong, native_1create)(JNIEnv *env, jobject thiz, jobject jtrack)
{
    MediaTrackRef * mediaTrack_ptr = reinterpret_cast<MediaTrackRef *>(JniUtils::getObjectPtr(env, jtrack));
    MediaAudioChannelRef * audioChannel_ptr = new MediaAudioChannelRef(new MediaAudioChannel((*mediaTrack_ptr).get()));

    return reinterpret_cast<jlong>(audioChannel_ptr);

}

NATIVE_FUNCTION(void, native_1release)(JNIEnv *env, jobject thiz)
{
    MediaAudioChannelRef * audioChannel_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, thiz));
    if (audioChannel_ptr)
        delete audioChannel_ptr;

    JniUtils::setObjectPtr(env, thiz, 0);
}

NATIVE_FUNCTION(jboolean , native_1isEnable)(JNIEnv *env, jobject thiz)
{
    MediaAudioChannelRef * audioChannel_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, thiz));
    bool enable = false;
    if (audioChannel_ptr)
        enable = (*audioChannel_ptr)->isEnable();

    return enable;
}

NATIVE_FUNCTION(void , native_1setEnable)(JNIEnv *env, jobject thiz, jboolean enable)
{
    MediaAudioChannelRef * audioChannel_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, thiz));
    if (audioChannel_ptr)
        (*audioChannel_ptr)->setEnable(enable);
}

NATIVE_FUNCTION(jfloat , native_1getVolume)(JNIEnv *env, jobject thiz)
{
    MediaAudioChannelRef * audioChannel_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, thiz));
    float volume = false;
    if (audioChannel_ptr)
        volume = (*audioChannel_ptr)->getVolume();

    return volume;
}

NATIVE_FUNCTION(void , native_1setVolume)(JNIEnv *env, jobject thiz, jfloat jvolume)
{
    MediaAudioChannelRef * audioChannel_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, thiz));
    if (audioChannel_ptr)
        (*audioChannel_ptr)->setVolume(jvolume);
}

NATIVE_FUNCTION(jfloat , native_1getPitch)(JNIEnv *env, jobject thiz)
{
    MediaAudioChannelRef * audioChannel_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, thiz));
    float pitch = false;
    if (audioChannel_ptr)
        pitch = (*audioChannel_ptr)->getPitch();

    return pitch;
}

NATIVE_FUNCTION(void , native_1setPitch)(JNIEnv *env, jobject thiz, jfloat jpitch)
{
    MediaAudioChannelRef * audioChannel_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, thiz));
    if (audioChannel_ptr)
        (*audioChannel_ptr)->setPitch(jpitch);
}