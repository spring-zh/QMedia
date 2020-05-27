//
// Created by spring on 2020/5/21.
//

#include "GraphicCore/GcLayer.h"
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QGraphic_QLayer_##name

NATIVE_FUNCTION(jlong, native_1create)(JNIEnv *env, jobject thiz, jobject jsize)
{
    GraphicCore::Size size(J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, jsize),
                           J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, jsize));
    GraphicCore::LayerRef * layer_ptr = new GraphicCore::LayerRef(new GraphicCore::Layer(size));

    return reinterpret_cast<jlong>(layer_ptr);
}

NATIVE_FUNCTION(jobject , native_1getBkColor)(JNIEnv *env, jobject thiz)
{
    GraphicCore::LayerRef * layer_ptr = reinterpret_cast<GraphicCore::LayerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (layer_ptr) {
        GraphicCore::Color4F bkcolor4F = (*layer_ptr)->getBKColor();
        return J4AC_com_qmedia_qmediasdk_QCommon_QVector__QVector4(env, bkcolor4F.r, bkcolor4F.g, bkcolor4F.b, bkcolor4F.a);
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QVector__QVector4(env, 0, 0, 0, 0);
}

NATIVE_FUNCTION(void , native_1setBkColor)(JNIEnv *env, jobject thiz, jobject jcolor)
{
    GraphicCore::LayerRef * layer_ptr = reinterpret_cast<GraphicCore::LayerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (layer_ptr) {
        GraphicCore::Color4F bkcolor4F(J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, jcolor),
                                     J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, jcolor),
                                     J4AC_com_qmedia_qmediasdk_QCommon_QVector__v2__get(env, jcolor),
                                     J4AC_com_qmedia_qmediasdk_QCommon_QVector__v3__get(env, jcolor));
        return (*layer_ptr)->setBKColor(bkcolor4F);
    }
}