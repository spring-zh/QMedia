//
// Created by spring on 2020/5/19.
//

#ifndef QMEDIASDK_QMEDIADESCRIBE_JNI_H
#define QMEDIASDK_QMEDIADESCRIBE_JNI_H

#include "JniUtils.h"
#include "MediaCore/core/MediaComm.h"

#define QAUDIODESCRIBE_CLASSPATH "com/qmedia/qmediasdk/QSource/QAudioDescribe"
#define QVIDEODESCRIBE_CLASSPATH "com/qmedia/qmediasdk/QSource/QVideoDescribe"

class QMediaDescribe_Jni {
public:
    static jobject VideoDescribeToJava(JNIEnv* env, VideoDescribe videoDescribe);
    static jobject AudioDescribeToJava(JNIEnv* env, AudioDescribe audioDescribe);

    static VideoDescribe JavaToVideoDescribe(JNIEnv* env, jobject jvideoDescribe);
    static AudioDescribe JavaToAudioDescribe(JNIEnv* env, jobject jaudioDescribe);
};


#endif //QMEDIASDK_QMEDIADESCRIBE_JNI_H
