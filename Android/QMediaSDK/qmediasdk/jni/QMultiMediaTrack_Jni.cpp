//
// Created by spring on 2020/5/20.
//

#include "EffectEditor/MediaTrackImpl.h"
#include "EffectEditor/EffectCombiner.h"
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"
#include "EffectEditor/sources/SoftwareSource.h"
#include "QMediaDescribe_Jni.h"

using EffectCombinerRef = std::shared_ptr<EffectCombiner>;

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QTrack_QMultiMediaTrack_##name

NATIVE_FUNCTION(jlong, native_1create)(JNIEnv *env, jobject thiz, jobject jfilelist, jobject jcombiner)
{
    jclass cls_list = env->GetObjectClass(jfilelist);
    /* method in class List  */
    jmethodID method_get = env->GetMethodID(cls_list, "get", "(I)Ljava/lang/Object;");
    jmethodID method_size = env->GetMethodID(cls_list, "size", "()I");
    /* jni invoke list.get to get points count */
    jint jlen = env->CallIntMethod(jfilelist, method_size);

    std::vector<std::string> filelist;
    for (int i = 0; i < jlen; ++i) {
        jstring jfile = static_cast<jstring>(env->CallObjectMethod(jfilelist, method_get, i));
        const char *file_str = env->GetStringUTFChars(jfile, NULL);
        filelist.push_back(file_str);
        env->ReleaseStringUTFChars(jfile, file_str);
    }

    EffectCombinerRef * combiner_ptr = reinterpret_cast<EffectCombinerRef *>(JniUtils::getObjectPtr(env, jcombiner));
    MediaSourceRef sourceRef = MediaSourceRef(new SoftwareSource(filelist));
    sourceRef->setVideoTarget((*combiner_ptr)->getVideoTarget());
    sourceRef->setAudioTarget((*combiner_ptr)->getAudioTarget());
    MediaTrackRef* track_ptr = new MediaTrackRef(new MediaTrackImpl(sourceRef));

    return reinterpret_cast<jlong>(track_ptr);

}

NATIVE_FUNCTION(jobject , native_1getVideoDescribe)(JNIEnv *env, jobject thiz)
{
    MediaTrackRef * track_ptr = reinterpret_cast<MediaTrackRef *>(JniUtils::getObjectPtr(env, thiz));
    if (track_ptr && (*track_ptr)->getVideoDescribe()) {
        return QMediaDescribe_Jni::VideoDescribeToJava(env, *(*track_ptr)->getVideoDescribe());
    }
    return NULL ;
}

NATIVE_FUNCTION(jobject , native_1getAudioDescribe)(JNIEnv *env, jobject thiz)
{
    MediaTrackRef * track_ptr = reinterpret_cast<MediaTrackRef *>(JniUtils::getObjectPtr(env, thiz));
    if (track_ptr && (*track_ptr)->getAudioDescribe()) {
        return QMediaDescribe_Jni::AudioDescribeToJava(env, *(*track_ptr)->getAudioDescribe());
    }
    return NULL ;
}
