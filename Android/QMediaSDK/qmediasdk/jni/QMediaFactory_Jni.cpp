//
// Created by spring on 2020/5/21.
//

#include "EffectEditor/EffectCombiner.h"

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QEditor_QMediaFactory_##name
#if 0
NATIVE_FUNCTION(jlong, native_create)(JNIEnv *env, jobject thiz)
{
    QMediaFactory_Jni *mediaFactory = new QMediaFactory_Jni();
    return reinterpret_cast<jlong>(mediaFactory);
}

NATIVE_FUNCTION(void, native_release)(JNIEnv *env, jobject thiz)
{
    QMediaFactory_Jni * mediaFactory = reinterpret_cast<QMediaFactory_Jni *>(JniUtils::getObjectPtr(env, thiz));
    if (mediaFactory)
        delete mediaFactory;

    JniUtils::setObjectPtr(env, thiz, 0);
}

NATIVE_FUNCTION(void, native_setVideoTarget)(JNIEnv *env, jobject thiz, jobject jtarget/*, jobject jcombiner*/)
{
    QMediaFactory_Jni * mediaFactory = reinterpret_cast<QMediaFactory_Jni *>(JniUtils::getObjectPtr(env, thiz));
    std::shared_ptr<EffectCombiner>* combiner_ptr = reinterpret_cast< std::shared_ptr<EffectCombiner> *>(JniUtils::getObjectPtr(env, jcombiner));
    if (mediaFactory) {
        mediaFactory->_videoTargetAdapter = std::shared_ptr<VideoTargetAdapter>(new VideoTargetAdapter(jtarget));
        if (combiner_ptr)
            (*combiner_ptr)->setVideoTarget(mediaFactory->_videoTargetAdapter.get());
    }
}

NATIVE_FUNCTION(void, native_setAudioTarget)(JNIEnv *env, jobject thiz, jobject jtarget/*, jobject jcombiner*/)
{
    QMediaFactory_Jni * mediaFactory = reinterpret_cast<QMediaFactory_Jni *>(JniUtils::getObjectPtr(env, thiz));
    std::shared_ptr<EffectCombiner>* combiner_ptr = reinterpret_cast< std::shared_ptr<EffectCombiner> *>(JniUtils::getObjectPtr(env, jcombiner));
    if (mediaFactory) {
        mediaFactory->_audioTargetAdapter = std::shared_ptr<AudioTargetAdapter>(new AudioTargetAdapter(jtarget));
        if (combiner_ptr)
            (*combiner_ptr)->setAudioTarget(mediaFactory->_audioTargetAdapter.get());
    }
}

#endif