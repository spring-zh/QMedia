//
// Created by spring on 2020/5/21.
//

#include "EffectEditor/EffectCombiner.h"
#include "MediaDescribe_Jni.h"
#include "j4a_generate/j4a_generate.h"

#define NATIVE_FUNCTION(retT, name)\
JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QEditor_QCombiner_##name

using EffectCombinerRef = std::shared_ptr<EffectCombiner>;

NATIVE_FUNCTION(void , native_setVideoConfig)(JNIEnv *env, jobject thiz, jobject jdescribe)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        (*combiner_ptr)->setvideoConfig(MediaDescribe_Jni::JavaToVideoDescribe(env, jdescribe));
    }
}
NATIVE_FUNCTION(void , native_setAudioConfig)(JNIEnv *env, jobject thiz, jobject jdescribe)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        (*combiner_ptr)->setAudioConfig(MediaDescribe_Jni::JavaToAudioDescribe(env, jdescribe));
    }
}

NATIVE_FUNCTION(void , native_addMediaTrack)(JNIEnv *env, jobject thiz, jobject jtrack)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    MediaTrackRef * mediaTrack_ptr = reinterpret_cast<MediaTrackRef *>(JniUtils::getObjectPtr(env, jtrack));
    if (combiner_ptr && mediaTrack_ptr) {
        (*combiner_ptr)->addMediaTrack(*mediaTrack_ptr);
    }
}

NATIVE_FUNCTION(void , native_removeMediaTrack)(JNIEnv *env, jobject thiz, jobject jtrack)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    MediaTrackRef * mediaTrack_ptr = reinterpret_cast<MediaTrackRef *>(JniUtils::getObjectPtr(env, jtrack));
    if (combiner_ptr && mediaTrack_ptr) {
        (*combiner_ptr)->removeMediaTrack(*mediaTrack_ptr);
    }
}

NATIVE_FUNCTION(jboolean , native_attachRenderNode)(JNIEnv *env, jobject thiz, jobject jchild, jobject jparent)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    GraphicCore::RenderNodeRef * childnode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jchild));
    GraphicCore::RenderNodeRef * parentnode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jparent));
    if (combiner_ptr && childnode_ptr && parentnode_ptr) {
        (*combiner_ptr)->attachRenderNode(*childnode_ptr, *parentnode_ptr);
        return 1;
    }
    return 0;
}
NATIVE_FUNCTION(jboolean , native_detachRenderNode)(JNIEnv *env, jobject thiz, jobject jcurrent)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    GraphicCore::RenderNodeRef * currentnode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jcurrent));
    if (combiner_ptr && currentnode_ptr ) {
        (*combiner_ptr)->detachRenderNode(*currentnode_ptr);
        return 1;
    }
    return 0;
}

NATIVE_FUNCTION(jlong , native_getPosition)(JNIEnv *env, jobject thiz)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        return (*combiner_ptr)->getPosition();
    }
    return 0;
}

NATIVE_FUNCTION(jobject , native_getMediaTimeRange)(JNIEnv *env, jobject thiz)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        Range<int64_t > mediaTimeRange = (*combiner_ptr)->getMediaTimeRange();
        return J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange(env, mediaTimeRange._start, mediaTimeRange._end);
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange(env, 0, 0);
}