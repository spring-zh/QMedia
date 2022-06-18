// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#pragma once

#include "djinni_support.hpp"
#include "media_stream_info.h"

namespace djinni_generated {

class NativeMediaStreamInfo final {
public:
    using CppType = ::QMedia::Api::MediaStreamInfo;
    using JniType = jobject;

    using Boxed = NativeMediaStreamInfo;

    ~NativeMediaStreamInfo();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    NativeMediaStreamInfo();
    friend ::djinni::JniClass<NativeMediaStreamInfo>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("com/qmedia/editor/generated/MediaStreamInfo") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(IIJJILjava/util/ArrayList;IILcom/qmedia/editor/generated/AudioDescription;Lcom/qmedia/editor/generated/VideoDescription;)V") };
    const jfieldID field_mMediaType { ::djinni::jniGetFieldID(clazz.get(), "mMediaType", "I") };
    const jfieldID field_mCodecType { ::djinni::jniGetFieldID(clazz.get(), "mCodecType", "I") };
    const jfieldID field_mDuration { ::djinni::jniGetFieldID(clazz.get(), "mDuration", "J") };
    const jfieldID field_mBitrate { ::djinni::jniGetFieldID(clazz.get(), "mBitrate", "J") };
    const jfieldID field_mStreamId { ::djinni::jniGetFieldID(clazz.get(), "mStreamId", "I") };
    const jfieldID field_mGops { ::djinni::jniGetFieldID(clazz.get(), "mGops", "Ljava/util/ArrayList;") };
    const jfieldID field_mTimebaseNum { ::djinni::jniGetFieldID(clazz.get(), "mTimebaseNum", "I") };
    const jfieldID field_mTimebaseDen { ::djinni::jniGetFieldID(clazz.get(), "mTimebaseDen", "I") };
    const jfieldID field_mADesc { ::djinni::jniGetFieldID(clazz.get(), "mADesc", "Lcom/qmedia/editor/generated/AudioDescription;") };
    const jfieldID field_mVDesc { ::djinni::jniGetFieldID(clazz.get(), "mVDesc", "Lcom/qmedia/editor/generated/VideoDescription;") };
};

}  // namespace djinni_generated
