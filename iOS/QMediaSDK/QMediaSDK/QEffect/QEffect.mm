//
//  QEffect.mm
//  QMediaSDK
//
//  Created by spring on 2020/6/18.
//  Copyright © 2020 QMedia. All rights reserved.
//

#import "QEffect_internal.h"

@implementation QEffect {
    GraphicCore::EffectRef _native;
}

- (instancetype)initWithNative:(GraphicCore::EffectRef)effect {
    if ((self = [self init]) != nil) {
        _native = effect;
    }
    return self;
}

- (GraphicCore::EffectRef)native {
    return _native;
}

- (NSString *)name {
    return [NSString stringWithUTF8String:_native->getConfig()->name.c_str()];
}

- (NSString *)describe {
    return [NSString stringWithUTF8String:_native->getConfig()->describe.c_str()];
}

- (NSString *)type {
    return [NSString stringWithUTF8String:_native->getConfig()->effect_type.c_str()];
}

- (NSRange)renderRange {
    NSRange nsRange = {static_cast<NSUInteger>(_native->_timeRange._start),
        static_cast<NSUInteger>(_native->_timeRange.getLength())};
    return nsRange;
}

- (void)setRenderRange:(NSRange)renderRange {
    Range<int64_t> range = {static_cast<long long>(renderRange.location),
        static_cast<long long>(renderRange.location + renderRange.length)};
    _native->_timeRange = range;
}

- (void)setIntValue:(NSString*)key value:(float)value {
    _native->setIntValue([key UTF8String], value);
}
- (void)setIntArrayValue:(NSString*)key value:(int[])value count:(int)count {
    _native->setIntArrayValue([key UTF8String], value, count);
}

- (void)setFloatValue:(NSString*)key value:(float)value {
    _native->setFloatValue([key UTF8String], value);
}
- (void)setFloatArrayValue:(NSString*)key value:(float[])value count:(int)count {
    _native->setFloatArrayValue([key UTF8String], value, count);
}

@end
