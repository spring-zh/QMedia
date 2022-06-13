// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_engine.djinni

#include "media_engine.h"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class QMediaEngine;

namespace djinni_generated {

class MediaEngine
{
public:
    using CppType = std::shared_ptr<::QMedia::Api::MediaEngine>;
    using CppOptType = std::shared_ptr<::QMedia::Api::MediaEngine>;
    using ObjcType = QMediaEngine*;

    using Boxed = MediaEngine;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

