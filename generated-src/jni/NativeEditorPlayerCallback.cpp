// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_session.djinni

#include "NativeEditorPlayerCallback.h"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeEditorPlayerCallback::NativeEditorPlayerCallback() : ::djinni::JniInterface<::QMedia::Api::EditorPlayerCallback, NativeEditorPlayerCallback>() {}

NativeEditorPlayerCallback::~NativeEditorPlayerCallback() = default;

NativeEditorPlayerCallback::JavaProxy::JavaProxy(JniType j) : Handle(::djinni::jniGetThreadEnv(), j) { }

NativeEditorPlayerCallback::JavaProxy::~JavaProxy() = default;

void NativeEditorPlayerCallback::JavaProxy::onPrepare(int32_t c_code) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeEditorPlayerCallback>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onPrepare,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_code)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeEditorPlayerCallback::JavaProxy::onStarted(int32_t c_code) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeEditorPlayerCallback>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onStarted,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_code)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeEditorPlayerCallback::JavaProxy::onStoped(int32_t c_code) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeEditorPlayerCallback>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onStoped,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_code)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeEditorPlayerCallback::JavaProxy::onSeekTo(int64_t c_time_ms) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeEditorPlayerCallback>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onSeekTo,
                           ::djinni::get(::djinni::I64::fromCpp(jniEnv, c_time_ms)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeEditorPlayerCallback::JavaProxy::onProgressUpdated(int64_t c_time_ms) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeEditorPlayerCallback>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onProgressUpdated,
                           ::djinni::get(::djinni::I64::fromCpp(jniEnv, c_time_ms)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeEditorPlayerCallback::JavaProxy::onCompleted(int32_t c_code) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeEditorPlayerCallback>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onCompleted,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_code)));
    ::djinni::jniExceptionCheck(jniEnv);
}
void NativeEditorPlayerCallback::JavaProxy::onEvent(int32_t c_eventid, const std::unordered_map<std::string, std::string> & c_msg) {
    auto jniEnv = ::djinni::jniGetThreadEnv();
    ::djinni::JniLocalScope jscope(jniEnv, 10);
    const auto& data = ::djinni::JniClass<::djinni_generated::NativeEditorPlayerCallback>::get();
    jniEnv->CallVoidMethod(Handle::get().get(), data.method_onEvent,
                           ::djinni::get(::djinni::I32::fromCpp(jniEnv, c_eventid)),
                           ::djinni::get(::djinni::Map<::djinni::String, ::djinni::String>::fromCpp(jniEnv, c_msg)));
    ::djinni::jniExceptionCheck(jniEnv);
}

}  // namespace djinni_generated