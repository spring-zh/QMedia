//
// Created by spring on 2020/5/19.
//

#include <android/log.h>
#include "JniUtils.h"
#include "j4a_generate/j4a_generate.h"

#define USE_JNI_VERION JNI_VERSION_1_6

extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    int retval;
    JNIEnv* env;
    if (vm->GetEnv((void **)&env, USE_JNI_VERION) != JNI_OK) {
        return -1;
    }

#ifdef __aarch64__
    __android_log_print(ANDROID_LOG_INFO,"xlmediasdk","jni so arch: aarch64");
#elif __arm__
    __android_log_print(ANDROID_LOG_INFO,"xlmediasdk","jni so arch: arm");
#endif

    JniUtils::setVM(vm);
    J4A_loadClass__J4AC_com_qmedia_qmediasdk_QCommon_QRange(env);
    J4A_loadClass__J4AC_com_qmedia_qmediasdk_QCommon_QVector(env);
    J4A_loadClass__J4AC_com_qmedia_qmediasdk_QSource_QAudioDescribe(env);
    J4A_loadClass__J4AC_com_qmedia_qmediasdk_QSource_QVideoDescribe(env);
    J4A_loadClass__J4AC_com_qmedia_qmediasdk_QAudio_QAudioFrame(env);
    J4A_loadClass__J4AC_com_qmedia_qmediasdk_QGraphic_QVideoFrame(env);

//    J4A_loadClass__J4AC_com_qmedia_qmediasdk_QGraphic_QNodeAnimator(env);
    return USE_JNI_VERION;

}

JNIEXPORT void JNI_OnUnload(JavaVM* pVm, void* reserved) {

    JNIEnv* env;
    if (pVm->GetEnv((void **)&env, USE_JNI_VERION) != JNI_OK) {
        return;
    }
}

}