// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from video_runloop.djinni

#pragma once

#include "djinni_support.hpp"
#include "video_render.h"

namespace djinni_generated {

class NativeVideoRender final : ::djinni::JniInterface<::QMedia::Api::VideoRender, NativeVideoRender> {
public:
    using CppType = std::shared_ptr<::QMedia::Api::VideoRender>;
    using CppOptType = std::shared_ptr<::QMedia::Api::VideoRender>;
    using JniType = jobject;

    using Boxed = NativeVideoRender;

    ~NativeVideoRender();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<NativeVideoRender>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<NativeVideoRender>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    NativeVideoRender();
    friend ::djinni::JniClass<NativeVideoRender>;
    friend ::djinni::JniInterface<::QMedia::Api::VideoRender, NativeVideoRender>;

};

}  // namespace djinni_generated
