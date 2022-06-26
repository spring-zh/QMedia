// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include "djinni_support.hpp"
#include "vec4f.h"

namespace djinni_generated {

class NativeVec4f final {
public:
    using CppType = ::QMedia::Api::Vec4f;
    using JniType = jobject;

    using Boxed = NativeVec4f;

    ~NativeVec4f();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeVec4f();
    friend ::djinni::JniClass<NativeVec4f>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/qmedia/editor/generated/Vec4f") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(FFFF)V") };
    const jfieldID field_mV1 { ::djinni::jniGetFieldID(clazz.get(), "mV1", "F") };
    const jfieldID field_mV2 { ::djinni::jniGetFieldID(clazz.get(), "mV2", "F") };
    const jfieldID field_mV3 { ::djinni::jniGetFieldID(clazz.get(), "mV3", "F") };
    const jfieldID field_mV4 { ::djinni::jniGetFieldID(clazz.get(), "mV4", "F") };
};

}  // namespace djinni_generated