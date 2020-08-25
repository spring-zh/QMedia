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
    GraphicCore::Size size(J4AC_com_qmedia_qmediasdk_QCommon_QSize__width__get(env, jsize),
                           J4AC_com_qmedia_qmediasdk_QCommon_QSize__height__get(env, jsize));
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

NATIVE_FUNCTION(jboolean , native_1isEnable3d)(JNIEnv *env, jobject thiz)
{
    GraphicCore::LayerRef * layer_ptr = reinterpret_cast<GraphicCore::LayerRef *>(JniUtils::getObjectPtr(env, thiz));
    jboolean jRet = JNI_FALSE;
    if (layer_ptr) {
        jRet = (*layer_ptr)->getEnable3d();
    }
    return jRet;
}

NATIVE_FUNCTION(void , native_1setEnable3d)(JNIEnv *env, jobject thiz, jboolean jenable)
{
    GraphicCore::LayerRef * layer_ptr = reinterpret_cast<GraphicCore::LayerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (layer_ptr) {
        return (*layer_ptr)->setEnable3d(jenable);
    }
}

NATIVE_FUNCTION(jint , native_1getFlipMode)(JNIEnv *env, jobject thiz)
{
    GraphicCore::LayerRef * layer_ptr = reinterpret_cast<GraphicCore::LayerRef *>(JniUtils::getObjectPtr(env, thiz));
    GraphicCore::Drawable2D::FlipMode flipMode = GraphicCore::Drawable2D::NONE;
    if (layer_ptr) {
        flipMode = (*layer_ptr)->getFlipMode();
    }
    return flipMode;
}

NATIVE_FUNCTION(void , native_1setFlipMode)(JNIEnv *env, jobject thiz, jint jmode)
{
    GraphicCore::LayerRef * layer_ptr = reinterpret_cast<GraphicCore::LayerRef *>(JniUtils::getObjectPtr(env, thiz));
    if (layer_ptr) {
        return (*layer_ptr)->setFlipMode((GraphicCore::Drawable2D::FlipMode)jmode);
    }
}