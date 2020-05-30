//
// Created by spring on 2020/5/21.
//

#include "QEditorPlayer_Jni.h"
#include "j4a_generate/j4a_generate.h"
#define QMEDIA_MUDULE "EditorPlayer"
#include "Utils/Logger.h"

using EditorPlayerRef = std::shared_ptr<EditorPlayer>;

QEditorPlayer_Jni::QEditorPlayer_Jni(jobject jpalyer): EditorPlayer() , _weakObj(jpalyer) {
    EditorPlayer::setCallBack(this);
}
QEditorPlayer_Jni::~QEditorPlayer_Jni() {
    _threadTask.postTask([this]() ->RetCode {
        _stop();
        _audioTarget->unInit();
        return RetCode::ok;
    });
    _threadTask.waitForStop();
}

void QEditorPlayer_Jni::onPrepare(RetCode code) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onPrepare(env, _weakObj, (jint)code);
}

void QEditorPlayer_Jni::onPlayerStateChanged(PlayerState newState, PlayerState oldState) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onPlayerStateChanged(env, _weakObj, (jint)newState, (jint)oldState);
}

void QEditorPlayer_Jni::onProgressUpdated(int64_t mSec) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onPlayerTimeProgressUpdated(env, _weakObj, (jlong)mSec);
}

void QEditorPlayer_Jni::onSeekTo(int64_t mSec) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onSeekTo(env, _weakObj, (jlong)mSec);
}

void QEditorPlayer_Jni::onCompleted() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QEditorPlayer__onCompleted(env, _weakObj, 0);
}

#pragma mark -----------NATIVE_FUNCTION---------------

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QEditor_QEditorPlayer_##name

NATIVE_FUNCTION(jlong, native_1create)(JNIEnv *env, jobject thiz, jobject jnode)
{
    EditorPlayerRef* editorPlayer_ptr = new EditorPlayerRef(new QEditorPlayer_Jni(thiz));
    JniUtils::setObjectPtr(env, jnode, (jlong)&((*editorPlayer_ptr)->getRootNode()));
    return reinterpret_cast<jlong>(editorPlayer_ptr);
}

NATIVE_FUNCTION(void , native_1release)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        delete editorPlayer_ptr;
    }
    JniUtils::setObjectPtr(env, thiz, 0);
}

NATIVE_FUNCTION(jint , native_1getPlayerState)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    PlayerState state = PlayerState::Idle;
    if (editorPlayer_ptr) {
        state = (*editorPlayer_ptr)->getPlayerState();
    }
    return (int)state;
}

NATIVE_FUNCTION(jboolean , native_1isPaused)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean paused = JNI_TRUE;
    if (editorPlayer_ptr) {
        paused = static_cast<jboolean>((*editorPlayer_ptr)->getUserPaused());
    }
    return paused;
}

NATIVE_FUNCTION(void , native_1start)(JNIEnv *env, jobject thiz)
{
    LOGI("native_1start");
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->prepare();
    }
}

NATIVE_FUNCTION(void , native_1play)(JNIEnv *env, jobject thiz)
{
    LOGI("native_1play");
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->start();
    }
}

NATIVE_FUNCTION(void , native_1pause)(JNIEnv *env, jobject thiz)
{
    LOGI("native_1pause");
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->pause(true);
    }
}

NATIVE_FUNCTION(void , native_1stop)(JNIEnv *env, jobject thiz)
{
    LOGI("native_1stop");
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->stop();
    }
}

NATIVE_FUNCTION(void , native_1seekTo)(JNIEnv *env, jobject thiz, jlong timePoint ,jint flags)
{
//    LOGI("native_1seekTo %ld", timePoint);
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    if (editorPlayer_ptr) {
        (*editorPlayer_ptr)->seek(timePoint, flags);
    }
}

//TODO: render
NATIVE_FUNCTION(jboolean , native_1onAudioRender)(JNIEnv *env, jobject thiz, jbyteArray jbuffer, jint jneedBytes, jlong jwantTime)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = JNI_FALSE;
    if (editorPlayer_ptr) {
        //FIXME: There are some problems with GetDirectBufferAddress
        jboolean is_copy = JNI_FALSE;
        uint8_t * pbuffer = reinterpret_cast<uint8_t *>(env->GetByteArrayElements(jbuffer, &is_copy));
#if 0
        uint8_t * pbuffer = static_cast<uint8_t *>(env->GetDirectBufferAddress(jbuffer));
        long buffersize = env->GetDirectBufferCapacity(jbuffer);
        if (buffersize < jneedBytes) {
            //TODO: buffer error
        }
#endif
        jret = ((AudioRender*)(*editorPlayer_ptr).get())->onAudioRender(pbuffer, jneedBytes, jwantTime);
        env->ReleaseByteArrayElements(jbuffer, reinterpret_cast<jbyte *>(pbuffer), is_copy? 0 : JNI_ABORT);
    }
    return jret;
}

NATIVE_FUNCTION(jboolean , native_1onVideoRender)(JNIEnv *env, jobject thiz, jlong jwantTime)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = JNI_FALSE;
    if (editorPlayer_ptr) {
        jret = ((VideoRender*)(*editorPlayer_ptr).get())->onVideoRender(jwantTime);
    }
    return jret;
}
NATIVE_FUNCTION(jboolean , native_1onVideoCreate)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = JNI_FALSE;
    if (editorPlayer_ptr) {
        jret = ((VideoRender*)(*editorPlayer_ptr).get())->onRenderCreate();
    }
    return jret;
}

NATIVE_FUNCTION(jboolean , native_1onVideoDestroy)(JNIEnv *env, jobject thiz)
{
    EditorPlayerRef* editorPlayer_ptr = reinterpret_cast<EditorPlayerRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = JNI_FALSE;
    if (editorPlayer_ptr) {
        jret = ((VideoRender*)(*editorPlayer_ptr).get())->onRenderDestroy();
    }
    return jret;
}