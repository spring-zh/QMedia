// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include "djinni_support.hpp"
#include "time_range.h"

namespace djinni_generated {

class NativeTimeRange final {
public:
    using CppType = ::QMedia::Api::TimeRange;
    using JniType = jobject;

    using Boxed = NativeTimeRange;

    ~NativeTimeRange();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeTimeRange();
    friend ::djinni::JniClass<NativeTimeRange>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/qmedia/editor/generated/TimeRange") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(JJ)V") };
    const jfieldID field_mStart { ::djinni::jniGetFieldID(clazz.get(), "mStart", "J") };
    const jfieldID field_mEnd { ::djinni::jniGetFieldID(clazz.get(), "mEnd", "J") };
};

}  // namespace djinni_generated
