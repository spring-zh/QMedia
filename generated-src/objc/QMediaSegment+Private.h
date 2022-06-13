// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_segment.djinni

#include "media_segment.h"
#include <memory>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@class QMediaSegment;

namespace djinni_generated {

class MediaSegment
{
public:
    using CppType = std::shared_ptr<::QMedia::Api::MediaSegment>;
    using CppOptType = std::shared_ptr<::QMedia::Api::MediaSegment>;
    using ObjcType = QMediaSegment*;

    using Boxed = MediaSegment;

    static CppType toCpp(ObjcType objc);
    static ObjcType fromCppOpt(const CppOptType& cpp);
    static ObjcType fromCpp(const CppType& cpp) { return fromCppOpt(cpp); }

private:
    class ObjcProxy;
};

}  // namespace djinni_generated

