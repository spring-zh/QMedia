// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include "audio_description.h"
#include "djinni_support.hpp"

namespace djinni_generated {

class NativeAudioDescription final {
public:
    using CppType = ::QMedia::Api::AudioDescription;
    using JniType = jobject;

    using Boxed = NativeAudioDescription;

    ~NativeAudioDescription();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeAudioDescription();
    friend ::djinni::JniClass<NativeAudioDescription>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/qmedia/editor/generated/AudioDescription") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(SSII)V") };
    const jfieldID field_mSmapleFormat { ::djinni::jniGetFieldID(clazz.get(), "mSmapleFormat", "S") };
    const jfieldID field_mNchannel { ::djinni::jniGetFieldID(clazz.get(), "mNchannel", "S") };
    const jfieldID field_mSampleRate { ::djinni::jniGetFieldID(clazz.get(), "mSampleRate", "I") };
    const jfieldID field_mSampleBits { ::djinni::jniGetFieldID(clazz.get(), "mSampleBits", "I") };
};

}  // namespace djinni_generated
