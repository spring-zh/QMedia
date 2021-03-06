//
// Created by spring on 2020/6/7.
//

#include "GraphicCore/GcImageNode.h"
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QGraphic_QImageNode_##name

NATIVE_FUNCTION(jlong, native_1create)(JNIEnv *env, jobject thiz, jstring jfilePath, jboolean jinAsset)
{
    const char * pfilePath = env->GetStringUTFChars(jfilePath, NULL);
    GraphicCore::ImageNodeRef * imageNode_ptr = new GraphicCore::ImageNodeRef(new GraphicCore::ImageNode(pfilePath));
    env->ReleaseStringUTFChars(jfilePath, pfilePath);

    return reinterpret_cast<jlong>(imageNode_ptr);
}

NATIVE_FUNCTION(jobject , native_1getSize)(JNIEnv *env, jobject thiz)
{
    GraphicCore::ImageNodeRef *imageNode_ptr = reinterpret_cast<GraphicCore::ImageNodeRef *>(JniUtils::getObjectPtr(
            env, thiz));
    int width = 0;
    int height = 0;
    if (imageNode_ptr){
        width = (*imageNode_ptr)->getWidth();
        height = (*imageNode_ptr)->getHeight();
    }
    return J4AC_com_qmedia_qmediasdk_QCommon_QSize__QSize(env, width, height);
}