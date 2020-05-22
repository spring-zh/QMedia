//
// Created by spring on 2020/5/21.
//

#include "GraphicCore/GcDuplicateNode.h"
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

#define NATIVE_FUNCTION(retT, name)\
JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QGraphic_QDuplicateNode_##name

NATIVE_FUNCTION(jlong, native_create)(JNIEnv *env, jobject thiz, jobject jnode)
{
    GraphicCore::RenderNodeRef * renderNode_ptr = reinterpret_cast<GraphicCore::RenderNodeRef *>(JniUtils::getObjectPtr(env, jnode));
    GraphicCore::DuplicateNodeRef * duplicateNode_ptr = new GraphicCore::DuplicateNodeRef(new GraphicCore::DuplicateNode((*renderNode_ptr).get()));

    return reinterpret_cast<jlong>(duplicateNode_ptr);
}