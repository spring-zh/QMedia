//
// Created by spring on 2020/5/21.
//

#include "QEditorPlayer_Jni.h"
#include "j4a_generate/j4a_generate.h"

using EditorPlayerRef = std::shared_ptr<EditorPlayer>;

QEditorPlayer_Jni::QEditorPlayer_Jni(jobject jpalyer): EditorPlayer() {
    JNIEnv* env = JniUtils::getEnv();
    _object = env->NewWeakGlobalRef(jpalyer);
}
QEditorPlayer_Jni::~QEditorPlayer_Jni() {
    JNIEnv* env = JniUtils::getEnv();
    env->DeleteWeakGlobalRef(_object);
}

void QEditorPlayer_Jni::onPrepare(RetCode code) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onPrepare(env, _object, (jint)code);
}

void QEditorPlayer_Jni::onPlayerStateChanged(PlayerState newState, PlayerState oldState) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onPlayerStateChanged(env, _object, (jint)newState, (jint)oldState);
}

void QEditorPlayer_Jni::onProgressUpdated(int64_t mSec) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onPlayerTimeProgressUpdated(env, _object, (jlong)mSec);
}

void QEditorPlayer_Jni::onSeekTo(int64_t mSec) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onSeekTo(env, _object, (jlong)mSec);
}

void QEditorPlayer_Jni::onCompleted() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onCompleted(env, _object, 0);
}

#pragma mark -----------NATIVE_FUNCTION---------------

#define NATIVE_FUNCTION(retT, name)\
JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QEditor_QEditorPlayer_##name

NATIVE_FUNCTION(jlong, native_create)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = new EditorPlayerRef(new QEditorPlayer_Jni(thiz));
    return reinterpret_cast<jlong>(editorPlayer_ptr);
}

NATIVE_FUNCTION(void , native_release)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        delete editorPlayer_ptr;
    }
    JniUtils::setObjectPtr(env, thiz, 0);
}

NATIVE_FUNCTION(jint , native_getPlayerState)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    PlayerState state = PlayerState::Idle;
    if (editorPlayer_ptr) {
        state = (*editorPlayer_ptr)->getPlayerState();
    }
    return (int)state;
}

NATIVE_FUNCTION(jboolean , native_isPaused)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean paused = 1;
    if (editorPlayer_ptr) {
        paused = static_cast<jboolean>((*editorPlayer_ptr)->getUserPaused());
    }
    return paused;
}

NATIVE_FUNCTION(void , native_start)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->prepare();
    }
}

NATIVE_FUNCTION(void , native_play)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->start();
    }
}

NATIVE_FUNCTION(void , native_pause)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->pause(true);
    }
}

NATIVE_FUNCTION(void , native_stop)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->stop();
    }
}

NATIVE_FUNCTION(void , native_seekTo)(JNIEnv *env, jobject thiz, jlong timePoint ,jint flags)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->seek(timePoint, flags);
    }
}

//TODO: render
NATIVE_FUNCTION(jboolean , native_onAudioRender)(JNIEnv *env, jobject thiz, jobject jbuffer, jint jneedBytes, jlong jwantTime)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = 0;
    if (editorPlayer_ptr) {
        uint8_t * pbuffer = static_cast<uint8_t *>(env->GetDirectBufferAddress(jbuffer));
        long buffersize = env->GetDirectBufferCapacity(jbuffer);
        if (buffersize < jneedBytes) {
            //TODO: buffer error
        }
        jret = ((AudioRender*)(*editorPlayer_ptr).get())->onAudioRender(pbuffer, jneedBytes, jwantTime);
    }
    return jret;
}

NATIVE_FUNCTION(jboolean , native_onVideoRender)(JNIEnv *env, jobject thiz, jlong jwantTime)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = 0;
    if (editorPlayer_ptr) {
        jret = ((VideoRender*)(*editorPlayer_ptr).get())->onVideoRender(jwantTime);
    }
    return jret;
}
NATIVE_FUNCTION(jboolean , native_onVideoCreate)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = 0;
    if (editorPlayer_ptr) {
        jret = ((VideoRender*)(*editorPlayer_ptr).get())->onRenderCreate();
    }
    return jret;
}

NATIVE_FUNCTION(jboolean , native_onVideoDestroy)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = 0;
    if (editorPlayer_ptr) {
        jret = ((VideoRender*)(*editorPlayer_ptr).get())->onRenderDestroy();
    }
    return jret;
}