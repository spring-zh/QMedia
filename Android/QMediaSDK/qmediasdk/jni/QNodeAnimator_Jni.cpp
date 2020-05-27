//
// Created by spring on 2020/5/20.
//

#include <GraphicCore/GcAnimaNode.h>
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

using GraphicCore::AnimaNode;

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QGraphic_QNodeAnimator_##name

NATIVE_FUNCTION(jlong, native_1create)(JNIEnv *env, jobject thiz, jstring jproperty, jobject jtimeRang, jobject jbeginPoint, jobject jendPoint,
                                      jint functionType, jboolean repleat ,jstring jname)
{
    AnimaNode::Animator* animator = new AnimaNode::Animator();
    const char* name = env->GetStringUTFChars(jname, NULL);
    animator->_name = std::string(name);
    env->ReleaseStringUTFChars(jname, name);

    const char* property = env->GetStringUTFChars(jproperty, NULL);
    animator->_property = std::string(property);
    env->ReleaseStringUTFChars(jproperty, property);

    animator->_timeRang._start = J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__get(env, jtimeRang);
    animator->_timeRang._end = J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__get(env, jtimeRang);

    GraphicCore::Vec4 begin_vec4(J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, jbeginPoint),
                           J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, jbeginPoint),
                           J4AC_com_qmedia_qmediasdk_QCommon_QVector__v2__get(env, jbeginPoint),
                           J4AC_com_qmedia_qmediasdk_QCommon_QVector__v3__get(env, jbeginPoint));
    animator->_beginValue = begin_vec4;
    GraphicCore::Vec4 end_vec4(J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, jendPoint),
                                 J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, jendPoint),
                                 J4AC_com_qmedia_qmediasdk_QCommon_QVector__v2__get(env, jendPoint),
                                 J4AC_com_qmedia_qmediasdk_QCommon_QVector__v3__get(env, jendPoint));
    animator->_endValue = end_vec4;
    animator->_functionType = (GraphicCore::EaseFunctionType)functionType;
    animator->_repleat = repleat;

    return reinterpret_cast<jlong>(animator);
}

NATIVE_FUNCTION(void , native_1release)(JNIEnv *env, jobject thiz)
{
    AnimaNode::Animator* animator = reinterpret_cast<AnimaNode::Animator*>(JniUtils::getObjectPtr(env, thiz));
    if (animator) {
        delete animator;
    }
    JniUtils::setObjectPtr(env, thiz, 0);
}