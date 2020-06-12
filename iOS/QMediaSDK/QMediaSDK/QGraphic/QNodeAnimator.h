//
//  QNodeAnimator.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef NS_ENUM(NSInteger, QEaseFunction) {
    CUSTOM_EASING = -1,
    
    Linear,
    
    Sine_EaseIn,
    Sine_EaseOut,
    Sine_EaseInOut,
    
    Quad_EaseIn,
    Quad_EaseOut,
    Quad_EaseInOut,
    
    Cubic_EaseIn,
    Cubic_EaseOut,
    Cubic_EaseInOut,
    
    Quart_EaseIn,
    Quart_EaseOut,
    Quart_EaseInOut,
    
    Quint_EaseIn,
    Quint_EaseOut,
    Quint_EaseInOut,
    
    Expo_EaseIn,
    Expo_EaseOut,
    Expo_EaseInOut,
    
    Circ_EaseIn,
    Circ_EaseOut,
    Circ_EaseInOut,
    
    Elastic_EaseIn,
    Elastic_EaseOut,
    Elastic_EaseInOut,
    
    Back_EaseIn,
    Back_EaseOut,
    Back_EaseInOut,
    
    Bounce_EaseIn,
    Bounce_EaseOut,
    Bounce_EaseInOut,
    
    TWEEN_EASING_MAX = 10000
};

#ifdef __cplusplus
}
#endif

/**
* QNodeAnimator
*/
extern NSString* property_positionxyz;//v3
extern NSString* property_rotation;//v1
extern NSString* property_rotationxyz;//v3
extern NSString* property_scalex;//v1
extern NSString* property_scalez;//v1
extern NSString* property_scalexy;//v2
extern NSString* property_scalexyz;//v3
extern NSString* property_contentsizew;//v1
extern NSString* property_contentsizeh;//v1
extern NSString* property_contentsize;//v2
extern NSString* property_alpha;//v1
extern NSString* property_mixcolor;//v4
extern NSString* property_crop;//v4 (0.0f ~ 1.0f)

@interface QNodeAnimator : NSObject
- (instancetype)initWith:(NSString*)property range:(NSRange)timeRang begin:(QVector)beginPoint end:(QVector)endPoint functype:(QEaseFunction)functionType repleat:(bool)repleat;
- (instancetype)initWith:(NSString*)property range:(NSRange)timeRang begin:(QVector)beginPoint end:(QVector)endPoint functype:(QEaseFunction)functionType repleat:(bool)repleat name:(NSString*)name;
@property (nonatomic, assign) NSString* property;
@property (nonatomic, assign) NSRange timeRang;
@property (nonatomic, assign) QVector beginPoint;
@property (nonatomic, assign) QVector endPoint;
@property (nonatomic, assign) QEaseFunction functionType;
@property (nonatomic, assign) NSString* name;
@property (nonatomic, assign) bool repleat;

@end
