//
// Created by spring on 2020/6/23.
//

#include "GraphicCore/effect/Effect.h"
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QEffect_QEffect_##name

NATIVE_FUNCTION(void , native_1release)(JNIEnv *env, jobject thiz)
{
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));
    if (effect_ptr) {
        delete effect_ptr;
    }
    JniUtils::setObjectPtr(env, thiz, 0);
}

NATIVE_FUNCTION(jstring , native_1getName)(JNIEnv *env, jobject thiz)
{
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));
    std::string name = "";
    if (effect_ptr) {
        name = (*effect_ptr)->getConfig()->name;
    }
    return env->NewStringUTF(name.c_str());
}

NATIVE_FUNCTION(jstring , native_1getDescribe)(JNIEnv *env, jobject thiz)
{
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));
    std::string name = "";
    if (effect_ptr) {
        name = (*effect_ptr)->getConfig()->describe;
    }
    return env->NewStringUTF(name.c_str());
}

NATIVE_FUNCTION(jstring , native_1getType)(JNIEnv *env, jobject thiz)
{
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));
    std::string name = "";
    if (effect_ptr) {
        name = (*effect_ptr)->getConfig()->effect_type;
    }
    return env->NewStringUTF(name.c_str());
}

NATIVE_FUNCTION(jobject , native_1getRenderRange)(JNIEnv *env, jobject thiz) {
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));
    Range<int64_t> range;
    if (effect_ptr) {
        range = (*effect_ptr)->_timeRange;
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange(env, range._start, range._end);
}

NATIVE_FUNCTION(void , native_1setRenderRange)(JNIEnv *env, jobject thiz, jobject jrenderRange) {
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));

    if (effect_ptr) {
        Range<int64_t> range = {J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__get(env, jrenderRange),
                                J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__get(env, jrenderRange)};
        (*effect_ptr)->_timeRange = range;
    }
}

NATIVE_FUNCTION(void , native_1setIntValue)(JNIEnv *env, jobject thiz, jstring jkey, jint jvalue) {
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));

    if (effect_ptr) {
        const char *key = env->GetStringUTFChars(jkey, NULL);
        (*effect_ptr)->setIntValue(key, jvalue);
        env->ReleaseStringUTFChars(jkey, key);
    }
}

NATIVE_FUNCTION(void , native_1setIntArrayValue)(JNIEnv *env, jobject thiz, jstring jkey, jintArray jvalue, jint jcount) {
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));

    if (effect_ptr) {
        const char *key = env->GetStringUTFChars(jkey, NULL);
        int* value_arr = env->GetIntArrayElements(jvalue, NULL);
        (*effect_ptr)->setIntArrayValue(key, value_arr, jcount);

        env->ReleaseIntArrayElements(jvalue, value_arr, JNI_ABORT);
        env->ReleaseStringUTFChars(jkey, key);
    }
}

NATIVE_FUNCTION(void , native_1setFloatValue)(JNIEnv *env, jobject thiz, jstring jkey, jfloat jvalue) {
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));

    if (effect_ptr) {
        const char *key = env->GetStringUTFChars(jkey, NULL);
        (*effect_ptr)->setFloatValue(key, jvalue);
        env->ReleaseStringUTFChars(jkey, key);
    }
}

NATIVE_FUNCTION(void , native_1setFloatArrayValue)(JNIEnv *env, jobject thiz, jstring jkey, jfloatArray jvalue, jint jcount) {
    GraphicCore::EffectRef * effect_ptr = reinterpret_cast<GraphicCore::EffectRef*>(JniUtils::getObjectPtr(env, thiz));

    if (effect_ptr) {
        const char *key = env->GetStringUTFChars(jkey, NULL);
        float* value_arr = env->GetFloatArrayElements(jvalue, NULL);
        (*effect_ptr)->setFloatArrayValue(key, value_arr, jcount);

        env->ReleaseFloatArrayElements(jvalue, value_arr, JNI_ABORT);
        env->ReleaseStringUTFChars(jkey, key);
    }
}