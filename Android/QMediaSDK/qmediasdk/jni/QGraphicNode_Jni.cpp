//
// Created by spring on 2020/5/20.
//

#include <GraphicCore/GcRenderNode.h>
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

using GraphicCore::AnimaNode;

#define NATIVE_FUNCTION(retT, name)\
JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QGraphic_QGraphicNode_##name

NATIVE_FUNCTION(jlong, native_create)(JNIEnv *env, jobject thiz, jstring jname)
{
    GraphicCore::RenderNodeRef* node_ptr = new GraphicCore::RenderNodeRef(new GraphicCore::RenderNode());
    const char* pname = env->GetStringUTFChars(jname, NULL);
    std::string name_str(pname);
    (*node_ptr)->setName(name_str);
    env->ReleaseStringUTFChars(jname, pname);
    return reinterpret_cast<jlong>(node_ptr);
}

NATIVE_FUNCTION(jboolean , native_addAnimator)(JNIEnv *env, jobject thiz, jobject janimator)
{
    GraphicCore::RenderNodeRef * node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, thiz));
    AnimaNode::Animator* animator = reinterpret_cast<AnimaNode::Animator*>(JniUtils::getObjectPtr(env, janimator));
    if (node_ptr && animator) {
        return (*node_ptr)->addAnimator(animator);
    }
    return false;
}

NATIVE_FUNCTION(jboolean , native_removeAnimator)(JNIEnv *env, jobject thiz, jobject janimator)
{
    GraphicCore::RenderNodeRef * node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, thiz));
    AnimaNode::Animator* animator = reinterpret_cast<AnimaNode::Animator*>(JniUtils::getObjectPtr(env, janimator));
    if (node_ptr && animator) {
        return (*node_ptr)->removeAnimator(animator);
    }
    return false;
}

NATIVE_FUNCTION(jobject , native_getRenderRange)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    Range<int64_t> range;
    if (node_ptr) {
        range = (*node_ptr)->getRange();
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QRange__QRange(env, range._start, range._end);
}

NATIVE_FUNCTION(void , native_setRenderRange)(JNIEnv *env, jobject thiz, jobject jrenderRange) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));

    if (node_ptr) {
        Range<int64_t> range = {J4AC_com_qmedia_qmediasdk_QCommon_QRange__start__get(env, jrenderRange),
                                J4AC_com_qmedia_qmediasdk_QCommon_QRange__end__get(env, jrenderRange)};
        (*node_ptr)->setRange(range);
    }
}

#pragma mark name
NATIVE_FUNCTION(jstring , native_getName)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    std::string name = "";
    if (node_ptr) {
        name = (*node_ptr)->getName();
    }
    return env->NewStringUTF(name.c_str());
}
NATIVE_FUNCTION(void , native_setName)(JNIEnv *env, jobject thiz, jstring jname) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    if (node_ptr) {
        const char *pname = env->GetStringUTFChars(jname, NULL);
        std::string name_str(pname);
        (*node_ptr)->setName(name_str);
        env->ReleaseStringUTFChars(jname, pname);
    }
}

#pragma mark Visible
NATIVE_FUNCTION(jboolean , native_isVisible)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    jboolean visible = false;
    if (node_ptr) {
        visible = (*node_ptr)->isVisible();
    }
    return visible;
}
NATIVE_FUNCTION(void , native_setVisible)(JNIEnv *env, jobject thiz, jboolean jvisible) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    if (node_ptr) {
        (*node_ptr)->setVisible(jvisible);
    }
}

#pragma mark Rotation
NATIVE_FUNCTION(jfloat , native_getRotation)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    jfloat rotation = 0;
    if (node_ptr) {
        rotation = (*node_ptr)->getRotation();
    }
    return rotation;
}

NATIVE_FUNCTION(void , native_setRotation)(JNIEnv *env, jobject thiz, jboolean jrotation) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    if (node_ptr) {
        (*node_ptr)->setRotation(jrotation);
    }
}
#pragma mark Rotation3d
NATIVE_FUNCTION(jobject , native_getRotation3d)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    GraphicCore::Vec3 rotation3d;
    if (node_ptr) {
        rotation3d = (*node_ptr)->getRotation3D();
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QVector__QVector3(env, rotation3d.x, rotation3d.y, rotation3d.z);
}

NATIVE_FUNCTION(void , native_setRotation3d)(JNIEnv *env, jobject thiz, jobject jrotation3d) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));

    if (node_ptr) {
        GraphicCore::Vec3 rotation3d = {J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, jrotation3d),
                                        J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, jrotation3d),
                                        J4AC_com_qmedia_qmediasdk_QCommon_QVector__v2__get(env, jrotation3d)};
        (*node_ptr)->setRotation3D(rotation3d);
    }
}

#pragma mark ScaleX
NATIVE_FUNCTION(jfloat , native_getScaleX)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    jfloat scaleX = 1;
    if (node_ptr) {
        scaleX = (*node_ptr)->getScaleX();
    }
    return scaleX;
}

NATIVE_FUNCTION(void , native_setScaleX)(JNIEnv *env, jobject thiz, jboolean jscaleX) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    if (node_ptr) {
        (*node_ptr)->setScaleX(jscaleX);
    }
}

#pragma mark ScaleY
NATIVE_FUNCTION(jfloat , native_getScaleY)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    jfloat scaleY = 1;
    if (node_ptr) {
        scaleY = (*node_ptr)->getScaleY();
    }
    return scaleY;
}

NATIVE_FUNCTION(void , native_setScaleY)(JNIEnv *env, jobject thiz, jboolean jscaleY) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    if (node_ptr) {
        (*node_ptr)->setScaleX(jscaleY);
    }
}

#pragma mark ScaleZ
NATIVE_FUNCTION(jfloat , native_getScaleZ)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    jfloat scaleZ = 1;
    if (node_ptr) {
        scaleZ = (*node_ptr)->getScaleY();
    }
    return scaleZ;
}

NATIVE_FUNCTION(void , native_setScaleZ)(JNIEnv *env, jobject thiz, jboolean jscaleZ) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    if (node_ptr) {
        (*node_ptr)->setScaleX(jscaleZ);
    }
}

#pragma mark ContentSize
NATIVE_FUNCTION(jobject , native_getContentSize)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    GraphicCore::Size contentSize;
    if (node_ptr) {
        contentSize = (*node_ptr)->getContentSize();
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QVector__QVector2(env, contentSize.width, contentSize.height);
}

NATIVE_FUNCTION(void , native_setContentSize)(JNIEnv *env, jobject thiz, jobject jcontentSize) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));

    if (node_ptr) {
        GraphicCore::Size contentSize = {J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, jcontentSize),
                                J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, jcontentSize)};
        (*node_ptr)->setContentSize(contentSize);
    }
}

#pragma mark Position
NATIVE_FUNCTION(jobject , native_getPosition)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    GraphicCore::Point position;
    if (node_ptr) {
        position = (*node_ptr)->getPosition();
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QVector__QVector2(env, position.x, position.y);
}

NATIVE_FUNCTION(void , native_setPosition)(JNIEnv *env, jobject thiz, jobject jposition) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));

    if (node_ptr) {
        GraphicCore::Point position = {J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, jposition),
                                         J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, jposition)};
        (*node_ptr)->setPosition(position);
    }
}

#pragma mark PositionZ
NATIVE_FUNCTION(jfloat , native_getPositionZ)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    jfloat positionZ = 0;
    if (node_ptr) {
        positionZ = (*node_ptr)->getPositionZ();
    }
    return positionZ;
}

NATIVE_FUNCTION(void , native_setPositionZ)(JNIEnv *env, jobject thiz, jboolean jpositionZ) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    if (node_ptr) {
        (*node_ptr)->setPositionZ(jpositionZ);
    }
}

#pragma mark AnchorPoint
NATIVE_FUNCTION(jobject , native_getAnchorPoint)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    GraphicCore::Point anchorPoint;
    if (node_ptr) {
        anchorPoint = (*node_ptr)->getAnchorPoint();
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QVector__QVector2(env, anchorPoint.x, anchorPoint.y);
}

NATIVE_FUNCTION(void , native_setAnchorPoint)(JNIEnv *env, jobject thiz, jobject janchorPoint) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));

    if (node_ptr) {
        GraphicCore::Point anchorPoint = {J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, janchorPoint),
                                       J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, janchorPoint)};
        (*node_ptr)->setAnchorPoint(anchorPoint);
    }
}

#pragma mark Color4
NATIVE_FUNCTION(jobject , native_getColor4)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    GraphicCore::Color4F color4;
    if (node_ptr) {
        color4 = (*node_ptr)->getColor();
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QVector__QVector4(env, color4.r, color4.g, color4.b, color4.a);
}

NATIVE_FUNCTION(void , native_setColor4)(JNIEnv *env, jobject thiz, jobject jcolor4) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));

    if (node_ptr) {
        GraphicCore::Color4F color4 = {J4AC_com_qmedia_qmediasdk_QCommon_QVector__v0__get(env, jcolor4),
                                        J4AC_com_qmedia_qmediasdk_QCommon_QVector__v1__get(env, jcolor4),
                                        J4AC_com_qmedia_qmediasdk_QCommon_QVector__v2__get(env, jcolor4),
                                        J4AC_com_qmedia_qmediasdk_QCommon_QVector__v3__get(env, jcolor4)};
        (*node_ptr)->setColor(color4);
    }
}

#pragma mark Alpha
NATIVE_FUNCTION(jfloat , native_getAlpha)(JNIEnv *env, jobject thiz) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    jfloat alpha = 0;
    if (node_ptr) {
        GraphicCore::Color4F color4 = (*node_ptr)->getColor();
        alpha = color4.a;
    }
    return alpha;
}

NATIVE_FUNCTION(void , native_setAlpha)(JNIEnv *env, jobject thiz, jboolean jalpha) {
    GraphicCore::RenderNodeRef *node_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    if (node_ptr) {
        GraphicCore::Color4F color4 = (*node_ptr)->getColor();
        color4.a = jalpha;
        (*node_ptr)->setColor(color4);
    }
}