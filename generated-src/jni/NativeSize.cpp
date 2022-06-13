// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#include "NativeSize.h"  // my header
#include "Marshal.hpp"

namespace djinni_generated {

NativeSize::NativeSize() = default;

NativeSize::~NativeSize() = default;

auto NativeSize::fromCpp(JNIEnv* jniEnv, const CppType& c) -> ::djinni::LocalRef<JniType> {
    const auto& data = ::djinni::JniClass<NativeSize>::get();
    auto r = ::djinni::LocalRef<JniType>{jniEnv->NewObject(data.clazz.get(), data.jconstructor,
                                                           ::djinni::get(::djinni::I64::fromCpp(jniEnv, c.width)),
                                                           ::djinni::get(::djinni::I64::fromCpp(jniEnv, c.height)))};
    ::djinni::jniExceptionCheck(jniEnv);
    return r;
}

auto NativeSize::toCpp(JNIEnv* jniEnv, JniType j) -> CppType {
    ::djinni::JniLocalScope jscope(jniEnv, 3);
    assert(j != nullptr);
    const auto& data = ::djinni::JniClass<NativeSize>::get();
    return {::djinni::I64::toCpp(jniEnv, jniEnv->GetLongField(j, data.field_mWidth)),
            ::djinni::I64::toCpp(jniEnv, jniEnv->GetLongField(j, data.field_mHeight))};
}

}  // namespace djinni_generated
