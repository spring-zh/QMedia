//
//  QFilterFactory.h
//  QMediaSDK
//
//  Created by spring on 2020/6/18.
//  Copyright © 2020 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "QEffect.h"

@interface QEffectConfig : NSObject
@property (nonatomic, readonly) NSString* name;
@property (nonatomic, readonly) NSString* describe;
@property (nonatomic, readonly) NSString* type;
@end

@interface QEffectManage : NSObject

+ (NSArray<QEffectConfig*>*)getAllEffectConfig;
+ (bool)addFilterConfig:(NSString*)json_config;

+ (QEffect*)createFilter:(NSString*)name;

@end
