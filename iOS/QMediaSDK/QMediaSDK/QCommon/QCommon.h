//
//  QCommon.h
//  QMediaSDK
//
//  Created by LINQIUMING on 05/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QCOMMON_H
#define QCOMMON_H

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct QVector {
    float v0, v1, v2, v3;
}QVector;
QVector QVectorV1(float v0);
QVector QVectorV2(float v0, float v1);
QVector QVectorV3(float v0, float v1, float v2);
QVector QVectorV4(float v0, float v1, float v2, float v3);
    
typedef struct QTimeRange{
    CGFloat startPoint;
    CGFloat duration;
} QTimeRange;

QTimeRange QTimeRangeMake(CGFloat startPoint, CGFloat duration);

NSString * QGetMediaSDKVersion();
    
#ifdef __cplusplus
}
#endif


@interface QCommonUtils : NSObject

+ (CGFloat)millisecondsToCGFloatSeconds:(int64_t)milliseconds;
+ (int64_t)CGFloatSecondsToMilliseconds:(CGFloat)seconds;

@end

#endif /* QCOMMON_H */
