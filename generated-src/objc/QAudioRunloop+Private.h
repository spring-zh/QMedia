// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from audio_runloop.djinni

#include "audio_runloop.h"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@protocol QAudioRunloop;

namespace djinni_generated {

class AudioRunloop
{
public:
    using CppType = std::shared_ptr<::QMedia::Api::AudioRunloop>;
    using CppOptType = std::shared_ptr<::QMedia::Api::AudioRunloop>;
    using ObjcType = id<QAudioRunloop>;

    using Boxed = AudioRunloop;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

