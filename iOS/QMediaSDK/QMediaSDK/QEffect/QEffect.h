//
//  QEffect.h
//  QMediaSDK
//
//  Created by spring on 2020/6/18.
//  Copyright © 2020 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface QEffect : NSObject

@property (nonatomic, readonly) NSString* name;
@property (nonatomic, readonly) NSString* describe;
@property (nonatomic, readonly) NSString* type;
@property (nonatomic) NSRange renderRange;

- (void)setIntValue:(NSString*)key value:(float)value;
- (void)setIntArrayValue:(NSString*)key value:(int[])value count:(int)count;
- (void)setFloatValue:(NSString*)key value:(float)value;
- (void)setFloatArrayValue:(NSString*)key value:(float[])value count:(int)count;

@end
