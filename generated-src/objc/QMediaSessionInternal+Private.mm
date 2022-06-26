// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_session.djinni

#import "QMediaSessionInternal+Private.h"
#import "QMediaSessionInternal.h"
#import "DJICppWrapperCache+Private.h"
#import "DJIError.h"
#import "DJIMarshal+Private.h"
#import "QAudioRender+Private.h"
#import "QAudioRunloop+Private.h"
#import "QMediaRange+Private.h"
#import "QMediaSegment+Private.h"
#import "QSize+Private.h"
#import "QVec4f+Private.h"
#import "QVideoRender+Private.h"
#import "QVideoRunloop+Private.h"
#include <exception>
#include <stdexcept>
#include <utility>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

@interface QMediaSessionInternal ()

- (id)initWithCpp:(const std::shared_ptr<::QMedia::Api::MediaSessionInternal>&)cppRef;

@end

@implementation QMediaSessionInternal {
    ::djinni::CppProxyCache::Handle<std::shared_ptr<::QMedia::Api::MediaSessionInternal>> _cppRefHandle;
}

- (id)initWithCpp:(const std::shared_ptr<::QMedia::Api::MediaSessionInternal>&)cppRef
{
    if (self = [super init]) {
        _cppRefHandle.assign(cppRef);
    }
    return self;
}

- (nullable QMediaSegment *)cresteMediaSegment:(nonnull NSString *)filename
                                          flag:(int32_t)flag {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->cresteMediaSegment(::djinni::String::toCpp(filename),
                                                                      ::djinni::I32::toCpp(flag));
        return ::djinni_generated::MediaSegment::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (BOOL)addMediaSegment:(nullable QMediaSegment *)segment {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->addMediaSegment(::djinni_generated::MediaSegment::toCpp(segment));
        return ::djinni::Bool::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (BOOL)removeMediaSegment:(nullable QMediaSegment *)segment {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->removeMediaSegment(::djinni_generated::MediaSegment::toCpp(segment));
        return ::djinni::Bool::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (nonnull NSArray<QMediaSegment *> *)getSegments {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->getSegments();
        return ::djinni::List<::djinni_generated::MediaSegment>::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (nonnull QMediaRange *)getTotalTimeRange {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->getTotalTimeRange();
        return ::djinni_generated::MediaRange::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (nullable QVideoRender *)getVideoRender {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->getVideoRender();
        return ::djinni_generated::VideoRender::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (nullable QAudioRender *)getAudioRender {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->getAudioRender();
        return ::djinni_generated::AudioRender::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setAudioRunLoop:(nullable id<QAudioRunloop>)audioLoop {
    try {
        _cppRefHandle.get()->setAudioRunLoop(::djinni_generated::AudioRunloop::toCpp(audioLoop));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setVideoRunLoop:(nullable id<QVideoRunloop>)videoLoop {
    try {
        _cppRefHandle.get()->setVideoRunLoop(::djinni_generated::VideoRunloop::toCpp(videoLoop));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setDisplayLayerSize:(nonnull QSize *)size {
    try {
        _cppRefHandle.get()->setDisplayLayerSize(::djinni_generated::Size::toCpp(size));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (nonnull QSize *)getDisplayLayerSize {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->getDisplayLayerSize();
        return ::djinni_generated::Size::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)setBkColor:(nonnull QVec4f *)color {
    try {
        _cppRefHandle.get()->setBkColor(::djinni_generated::Vec4f::toCpp(color));
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (nonnull QVec4f *)getBkColor {
    try {
        auto objcpp_result_ = _cppRefHandle.get()->getBkColor();
        return ::djinni_generated::Vec4f::fromCpp(objcpp_result_);
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)prepare {
    try {
        _cppRefHandle.get()->prepare();
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

- (void)stop {
    try {
        _cppRefHandle.get()->stop();
    } DJINNI_TRANSLATE_EXCEPTIONS()
}

namespace djinni_generated {

auto MediaSessionInternal::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return objc->_cppRefHandle.get();
}

auto MediaSessionInternal::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return ::djinni::get_cpp_proxy<QMediaSessionInternal>(cpp);
}

}  // namespace djinni_generated

@end
