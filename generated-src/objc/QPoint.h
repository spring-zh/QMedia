// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#import <Foundation/Foundation.h>
#import <Foundation/NSDictionary.h>

@interface QPoint : NSObject
- (nonnull instancetype)initWithX:(float)x
                                y:(float)y;
+ (nonnull instancetype)pointWithX:(float)x
                                 y:(float)y;
- (nonnull instancetype)initWithJson:(nonnull NSDictionary*)json;
- (nonnull NSDictionary*)toJson;

@property (nonatomic ) float x;

@property (nonatomic ) float y;

@end