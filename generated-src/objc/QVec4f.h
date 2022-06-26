// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#import <Foundation/Foundation.h>
#import <Foundation/NSDictionary.h>

@interface QVec4f : NSObject
- (nonnull instancetype)initWithV1:(float)v1
                                v2:(float)v2
                                v3:(float)v3
                                v4:(float)v4;
+ (nonnull instancetype)vec4fWithV1:(float)v1
                                 v2:(float)v2
                                 v3:(float)v3
                                 v4:(float)v4;
- (nonnull instancetype)initWithJson:(nonnull NSDictionary*)json;
- (nonnull NSDictionary*)toJson;

@property (nonatomic ) float v1;

@property (nonatomic ) float v2;

@property (nonatomic ) float v3;

@property (nonatomic ) float v4;

@end
