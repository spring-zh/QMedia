//
// Created by spring on 2020/5/21.
//

#include "EffectEditor/MediaGraphicChannel.h"
#include "EffectEditor/MediaTrackImpl.h"
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QGraphic_QVideoTrackNode_##name

NATIVE_FUNCTION(jlong, native_1create)(JNIEnv *env, jobject thiz, jobject jtrack)
{
    MediaTrackRef * mediaTrack_ptr = reinterpret_cast<MediaTrackRef *>(JniUtils::getObjectPtr(env, jtrack));
    MediaGraphicChannelRef * graphicChannel_ptr = new MediaGraphicChannelRef(new MediaGraphicChannel((*mediaTrack_ptr).get()));

    return reinterpret_cast<jlong>(graphicChannel_ptr);

}
