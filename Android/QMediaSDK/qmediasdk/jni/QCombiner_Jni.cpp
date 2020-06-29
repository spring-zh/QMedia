//
// Created by spring on 2020/5/21.
//

#include "EffectEditor/EffectCombiner.h"
#include "QMediaDescribe_Jni.h"
#include "j4a_generate/j4a_generate.h"
#include "VideoTargetAdapter.h"
#include "AudioTargetAdapter.h"

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QEditor_QCombiner_##name

using EffectCombinerRef = std::shared_ptr<EffectCombiner>;
using VideoTargetAdapterRef = std::shared_ptr<VideoTargetAdapter>;
using AudioTargetAdapterRef = std::shared_ptr<AudioTargetAdapter>;

NATIVE_FUNCTION(void , native_1setVideoConfig)(JNIEnv *env, jobject thiz, jobject jdescribe)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        (*combiner_ptr)->setvideoConfig(QMediaDescribe_Jni::JavaToVideoDescribe(env, jdescribe));
    }
}
NATIVE_FUNCTION(void , native_1setAudioConfig)(JNIEnv *env, jobject thiz, jobject jdescribe)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        (*combiner_ptr)->setAudioConfig(QMediaDescribe_Jni::JavaToAudioDescribe(env, jdescribe));
    }
}

NATIVE_FUNCTION(void , native_1setVideoTarget)(JNIEnv *env, jobject thiz, jobject jtarget)
{
    VideoTargetAdapterRef* videoTarget_ptr = new VideoTargetAdapterRef(new VideoTargetAdapter(jtarget));
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    JniUtils::setObjectPtr(env, thiz, (jlong)videoTarget_ptr, "mVideoTargetPtr");
    combiner_ptr->get()->setVideoTarget((*videoTarget_ptr).get());
}
NATIVE_FUNCTION(void , native_1setAudioTarget)(JNIEnv *env, jobject thiz, jobject jtarget)
{
    AudioTargetAdapterRef* audoiTarget_ptr = new AudioTargetAdapterRef(new AudioTargetAdapter(jtarget));
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    JniUtils::setObjectPtr(env, thiz, (jlong)audoiTarget_ptr, "mAudioTargetPtr");
    combiner_ptr->get()->setAudioTarget((*audoiTarget_ptr).get());
}

NATIVE_FUNCTION(void , native_1addMediaTrack)(JNIEnv *env, jobject thiz, jobject jtrack)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    MediaTrackRef * mediaTrack_ptr = reinterpret_cast<MediaTrackRef *>(JniUtils::getObjectPtr(env, jtrack));
    if (combiner_ptr && mediaTrack_ptr) {
        (*combiner_ptr)->addMediaTrack(*mediaTrack_ptr);
    }
}

NATIVE_FUNCTION(void , native_1removeMediaTrack)(JNIEnv *env, jobject thiz, jobject jtrack)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    MediaTrackRef * mediaTrack_ptr = reinterpret_cast<MediaTrackRef *>(JniUtils::getObjectPtr(env, jtrack));
    if (combiner_ptr && mediaTrack_ptr) {
        (*combiner_ptr)->removeMediaTrack(*mediaTrack_ptr);
    }
}

NATIVE_FUNCTION(jboolean , native_1attachRenderNode)(JNIEnv *env, jobject thiz, jobject jchild, jobject jparent)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    GraphicCore::RenderNodeRef * childnode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jchild));
    GraphicCore::RenderNodeRef * parentnode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jparent));
    if (combiner_ptr && childnode_ptr && parentnode_ptr) {
        (*combiner_ptr)->attachRenderNode(*childnode_ptr, *parentnode_ptr);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}
NATIVE_FUNCTION(jboolean , native_1detachRenderNode)(JNIEnv *env, jobject thiz, jobject jcurrent)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    GraphicCore::RenderNodeRef * currentnode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jcurrent));
    if (combiner_ptr && currentnode_ptr ) {
        (*combiner_ptr)->detachRenderNode(*currentnode_ptr);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

NATIVE_FUNCTION(jboolean , native_1attachEffect)(JNIEnv *env, jobject thiz, jobject jnode, jobject jeffect)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    GraphicCore::RenderNodeRef * renderNode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jnode));
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef *>(JniUtils::getObjectPtr(env, jeffect));
    if (combiner_ptr && renderNode_ptr && effect_ptr) {
        (*combiner_ptr)->attachEffect(*renderNode_ptr, *effect_ptr);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}
NATIVE_FUNCTION(jboolean , native_1detachEffect)(JNIEnv *env, jobject thiz, jobject jnode, jobject jeffect)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    GraphicCore::RenderNodeRef * renderNode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jnode));
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef *>(JniUtils::getObjectPtr(env, jeffect));
    if (combiner_ptr && renderNode_ptr && effect_ptr) {
        (*combiner_ptr)->detachEffect(*renderNode_ptr, *effect_ptr);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

NATIVE_FUNCTION(jboolean , native_1attachAudioNode)(JNIEnv *env, jobject thiz, jobject jchild, jobject jparent)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    MediaAudioChannelRef * childnode_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, jchild));
    if (combiner_ptr && childnode_ptr) {
        (*combiner_ptr)->attachAudioNode(*childnode_ptr, nullptr);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}
NATIVE_FUNCTION(jboolean , native_1detachAudioNode)(JNIEnv *env, jobject thiz, jobject jcurrent)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    MediaAudioChannelRef * currentnode_ptr = reinterpret_cast<MediaAudioChannelRef *>(JniUtils::getObjectPtr(env, jcurrent));
    if (combiner_ptr && currentnode_ptr ) {
        (*combiner_ptr)->detachAudioNode(*currentnode_ptr);
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

NATIVE_FUNCTION(jlong , native_1getPosition)(JNIEnv *env, jobject thiz)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        return (*combiner_ptr)->getPosition();
    }
    return 0;
}

NATIVE_FUNCTION(void , native_1setValidTimeRange)(JNIEnv *env, jobject thiz, jobject jtimeRange)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        Range<int64_t> range = {J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__get(env, jtimeRange),
                                J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__get(env, jtimeRange)};
        (*combiner_ptr)->setValidTimeRange(range);
    }
}

NATIVE_FUNCTION(jobject , native_1getMediaTimeRange)(JNIEnv *env, jobject thiz)
{
    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (combiner_ptr) {
        Range<int64_t > mediaTimeRange = (*combiner_ptr)->getValidTimeRange();
        return J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange(env, mediaTimeRange._start, mediaTimeRange._end);
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange(env, 0, 0);
}

NATIVE_FUNCTION(void, native_1target_1release)(JNIEnv *env, jobject thiz)
{
    VideoTargetAdapterRef* videoTarget_ptr = reinterpret_cast<VideoTargetAdapterRef *>(JniUtils::getObjectPtr(env, thiz, "mVideoTargetPtr"));
    if (videoTarget_ptr) delete videoTarget_ptr;
    JniUtils::setObjectPtr(env, thiz, 0, "mVideoTargetPtr");
    AudioTargetAdapterRef* audioTarget_ptr = reinterpret_cast<AudioTargetAdapterRef *>(JniUtils::getObjectPtr(env, thiz, "mAudioTargetPtr"));
    if (audioTarget_ptr) delete audioTarget_ptr;
    JniUtils::setObjectPtr(env, thiz, 0, "mAudioTargetPtr");
}