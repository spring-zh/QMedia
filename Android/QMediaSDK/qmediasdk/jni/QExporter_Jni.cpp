//
// Created by spring on 2020/5/29.
//

#include "QExporter_Jni.h"
#include "j4a_generate/j4a_generate.h"
#define QMEDIA_MUDULE "QExporter"
#include "Utils/Logger.h"

using ExporterRef = std::shared_ptr<EditorExporter>;

QExporter_Jni::QExporter_Jni(jobject jexporter): EditorExporter(), _weakObj(jexporter) {
    EditorExporter::setCallBack(this);
}
QExporter_Jni::~QExporter_Jni() {
    _threadTask.postTask([this]() ->RetCode {
        _stop();
        _audioTarget->unInit();
        return RetCode::ok;
    });
    _threadTask.waitForStop();
}

void QExporter_Jni::onStarted(RetCode code) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QExporter__onExporterStarted(env, _weakObj, code);
}

void QExporter_Jni::onStoped(RetCode code) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QExporter__onExporterStoped(env, _weakObj, code);
}

void QExporter_Jni::onProgressUpdated(int64_t mSec) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QExporter__onExporterProgressUpdated(env, _weakObj, mSec);
}

void QExporter_Jni::onCanceled(RetCode code) {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QExporter__onExporterCanceled(env, _weakObj, code);
}

void QExporter_Jni::onCompleted() {
    JNIEnv* env = JniUtils::getEnv();
    J4AC_com_qmedia_qmediasdk_QEditor_QExporter__onExporterCompleted(env, _weakObj);
}

#pragma mark -----------NATIVE_FUNCTION---------------

#define NATIVE_FUNCTION(retT, name)\
extern "C" JNIEXPORT retT JNICALL Java_com_qmedia_qmediasdk_QEditor_QExporter_##name

NATIVE_FUNCTION(jlong, native_1create)(JNIEnv *env, jobject thiz, jobject jnode)
{
    ExporterRef* exporter_ptr = new ExporterRef(new QExporter_Jni(thiz));
    std::shared_ptr<DisplayLayer>* renderLayer_ptr = new std::shared_ptr<DisplayLayer>((*exporter_ptr)->getRootNode());
    JniUtils::setObjectPtr(env, jnode, (jlong)renderLayer_ptr);
    return reinterpret_cast<jlong>(exporter_ptr);
}

NATIVE_FUNCTION(void , native_1release)(JNIEnv *env, jobject thiz)
{
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    if (exporter_ptr) {
        delete exporter_ptr;
    }
    JniUtils::setObjectPtr(env, thiz, 0);
}

NATIVE_FUNCTION(void , native_1start)(JNIEnv *env, jobject thiz)
{
    LOGI("native_1start");
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    if (exporter_ptr) {
        (*exporter_ptr)->prepare();
    }
}

NATIVE_FUNCTION(void , native_1cancel)(JNIEnv *env, jobject thiz)
{
    LOGI("native_1play");
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    if (exporter_ptr) {
        (*exporter_ptr)->cancel();
    }
}

NATIVE_FUNCTION(void , native_1stop)(JNIEnv *env, jobject thiz)
{
    LOGI("native_1stop");
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    if (exporter_ptr) {
        (*exporter_ptr)->stop();
    }
}

//TODO: render
NATIVE_FUNCTION(jboolean , native_1onAudioRender)(JNIEnv *env, jobject thiz, jbyteArray jbuffer, jint jneedBytes, jlong jwantTime)
{
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = JNI_FALSE;
    if (exporter_ptr) {
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
        jret = ((AudioRender*)(*exporter_ptr).get())->onAudioRender(pbuffer, jneedBytes, jwantTime);
        env->ReleaseByteArrayElements(jbuffer, reinterpret_cast<jbyte *>(pbuffer), is_copy? 0 : JNI_ABORT);
    }
    return jret;
}

NATIVE_FUNCTION(jboolean , native_1onVideoRender)(JNIEnv *env, jobject thiz, jlong jwantTime)
{
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = JNI_FALSE;
    if (exporter_ptr) {
        jret = ((VideoRender*)(*exporter_ptr).get())->onVideoRender(jwantTime);
    }
    return jret;
}
NATIVE_FUNCTION(jboolean , native_1onVideoCreate)(JNIEnv *env, jobject thiz)
{
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = JNI_FALSE;
    if (exporter_ptr) {
        jret = ((VideoRender*)(*exporter_ptr).get())->onRenderCreate();
    }
    return jret;
}

NATIVE_FUNCTION(jboolean , native_1onVideoDestroy)(JNIEnv *env, jobject thiz)
{
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    jboolean jret = JNI_FALSE;
    if (exporter_ptr) {
        jret = ((VideoRender*)(*exporter_ptr).get())->onRenderDestroy();
    }
    return jret;
}

NATIVE_FUNCTION(void , native_1setDisplayMode)(JNIEnv *env, jobject thiz, jint jdisplayMode, jint jviewW, jint jviewH)
{
    ExporterRef* exporter_ptr = reinterpret_cast<ExporterRef*>(JniUtils::getObjectPtr(env, thiz));
    if (exporter_ptr) {
        ((VideoRender*)(*exporter_ptr).get())->setDisplayMode((DisplayMode)jdisplayMode, jviewW, jviewH);
    }
}