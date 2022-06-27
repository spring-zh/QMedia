// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from video_runloop.djinni

#include "NativeVideoRender.h"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeVideoRender::NativeVideoRender() : ::djinni::JniInterface<::QMedia::Api::VideoRender, NativeVideoRender>("com/qmedia/editor/generated/VideoRender$CppProxy") {}

NativeVideoRender::~NativeVideoRender() = default;


CJNIEXPORT void JNICALL Java_com_qmedia_editor_generated_VideoRender_00024CppProxy_nativeDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        delete reinterpret_cast<::djinni::CppProxyHandle<::QMedia::Api::VideoRender>*>(nativeRef);
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_com_qmedia_editor_generated_VideoRender_00024CppProxy_native_1setDisplayMode(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jint j_mode, jboolean j_flipV)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::QMedia::Api::VideoRender>(nativeRef);
        ref->setDisplayMode(::djinni::I32::toCpp(jniEnv, j_mode),
                            ::djinni::Bool::toCpp(jniEnv, j_flipV));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_com_qmedia_editor_generated_VideoRender_00024CppProxy_native_1onViewSizeChange(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jint j_width, jint j_height)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::QMedia::Api::VideoRender>(nativeRef);
        ref->onViewSizeChange(::djinni::I32::toCpp(jniEnv, j_width),
                              ::djinni::I32::toCpp(jniEnv, j_height));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT void JNICALL Java_com_qmedia_editor_generated_VideoRender_00024CppProxy_native_1onViewDestroy(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::QMedia::Api::VideoRender>(nativeRef);
        ref->onViewDestroy();
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

CJNIEXPORT jboolean JNICALL Java_com_qmedia_editor_generated_VideoRender_00024CppProxy_native_1onDraw(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jlong j_pirv, jboolean j_noDisplay)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::QMedia::Api::VideoRender>(nativeRef);
        auto r = ref->onDraw(::djinni::I64::toCpp(jniEnv, j_pirv),
                             ::djinni::Bool::toCpp(jniEnv, j_noDisplay));
        return ::djinni::release(::djinni::Bool::fromCpp(jniEnv, r));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, 0 /* value doesn't matter */)
}

CJNIEXPORT void JNICALL Java_com_qmedia_editor_generated_VideoRender_00024CppProxy_native_1readRGBA(JNIEnv* jniEnv, jobject /*this*/, jlong nativeRef, jobject j_buffer, jint j_width, jint j_height, jint j_format)
{
    try {
        DJINNI_FUNCTION_PROLOGUE1(jniEnv, nativeRef);
        const auto& ref = ::djinni::objectFromHandleAddress<::QMedia::Api::VideoRender>(nativeRef);
        ref->readRGBA(::djinni::Object::toCpp(jniEnv, j_buffer),
                      ::djinni::I32::toCpp(jniEnv, j_width),
                      ::djinni::I32::toCpp(jniEnv, j_height),
                      ::djinni::I32::toCpp(jniEnv, j_format));
    } JNI_TRANSLATE_EXCEPTIONS_RETURN(jniEnv, )
}

}  // namespace djinni_generated
