// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_session.djinni

#import "QEditorPlayerCallback+Private.h"
#import "QEditorPlayerCallback.h"
#import "DJIMarshal+Private.h"
#import "DJIObjcWrapperCache+Private.h"
#include <stdexcept>

static_assert(__has_feature(objc_arc), "Djinni requires ARC to be enabled for this file");

namespace djinni_generated {

class EditorPlayerCallback::ObjcProxy final
: public ::QMedia::Api::EditorPlayerCallback
, private ::djinni::ObjcProxyBase<ObjcType>
{
    friend class ::djinni_generated::EditorPlayerCallback;
public:
    using ObjcProxyBase::ObjcProxyBase;
    void onPrepare(int32_t c_code) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onPrepare:(::djinni::I32::fromCpp(c_code))];
        }
    }
    void onStarted(int32_t c_code) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onStarted:(::djinni::I32::fromCpp(c_code))];
        }
    }
    void onStoped(int32_t c_code) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onStoped:(::djinni::I32::fromCpp(c_code))];
        }
    }
    void onSeekTo(int64_t c_time_ms) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onSeekTo:(::djinni::I64::fromCpp(c_time_ms))];
        }
    }
    void onProgressUpdated(int64_t c_time_ms) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onProgressUpdated:(::djinni::I64::fromCpp(c_time_ms))];
        }
    }
    void onPlayerStateChanged(int32_t c_new_state, int32_t c_old_state) override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onPlayerStateChanged:(::djinni::I32::fromCpp(c_new_state))
                                                                  oldState:(::djinni::I32::fromCpp(c_old_state))];
        }
    }
    void onCompleted() override
    {
        @autoreleasepool {
            [djinni_private_get_proxied_objc_object() onCompleted];
        }
    }
};

}  // namespace djinni_generated

namespace djinni_generated {

auto EditorPlayerCallback::toCpp(ObjcType objc) -> CppType
{
    if (!objc) {
        return nullptr;
    }
    return ::djinni::get_objc_proxy<ObjcProxy>(objc);
}

auto EditorPlayerCallback::fromCppOpt(const CppOptType& cpp) -> ObjcType
{
    if (!cpp) {
        return nil;
    }
    return dynamic_cast<ObjcProxy&>(*cpp).djinni_private_get_proxied_objc_object();
}

}  // namespace djinni_generated
