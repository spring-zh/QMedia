//
//  QFilterFactory.mm
//  QMediaSDK
//
//  Created by spring on 2020/6/18.
//  Copyright © 2020 QMedia. All rights reserved.
//

#import "QEffectManage.h"
#import "QEffect_internal.h"
#include "GraphicCore/effect/EffectManage.h"

using EffectManage = GraphicCore::EffectManage;

@implementation QEffectConfig {
    NSString* _name;
    NSString* _describe;
    NSString* _type;
}
- (instancetype)initWith:(NSString*)name describe:(NSString*)describe type:(NSString*)type{
    if ((self = [super init]) != nil) {
        _name = name;
        _describe = describe;
        _type = type;
    }
    return self;;
}
- (NSString *)name {
    return _name;
}
- (NSString *)describe {
    return _describe;
}
- (NSString *)type {
    return _type;;
}
@end

@implementation QEffectManage

+ (NSArray<QEffectConfig*>*)getAllEffectConfig {
    NSMutableArray<QEffectConfig*>* nsconfigs = [NSMutableArray new];
    auto allConfig = EffectManage::instance()->getAllEffectConfig();
    for (auto& config : allConfig) {
        NSString* name = [NSString stringWithUTF8String:config->name.c_str()];
        NSString* describe = [NSString stringWithUTF8String:config->describe.c_str()];
        NSString* type = [NSString stringWithUTF8String:config->effect_type.c_str()];
        [nsconfigs addObject:[[QEffectConfig alloc] initWith:name describe:describe type:type]];
    }
    return nsconfigs;
}

+ (bool)addFilterConfig:(NSString*)json_config {
    std::string config_str = [json_config UTF8String];
    EffectManage::instance()->addEffectConfig(config_str);
    return true;
}

+ (QEffect*)createEffect:(NSString*)name {
    GraphicCore::Effect* effect = EffectManage::instance()->createEffect([name UTF8String]);
    if (effect) {
        return [[QEffect alloc] initWithNative:GraphicCore::EffectRef(effect)];
    }else
        return nil;
}

@end
