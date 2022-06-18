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

#import "QMediaRange.h"

#ifdef __cplusplus
extern "C" {
#endif

//QVector
typedef struct QVector {
    float v0, v1, v2, v3;
}QVector;
QVector QVectorV1(float v0);
QVector QVectorV2(float v0, float v1);
QVector QVectorV3(float v0, float v1, float v2);
QVector QVectorV4(float v0, float v1, float v2, float v3);
    
//QTimeRange
typedef struct QTimeRange{
    int64_t startPoint;
    int64_t endPoint;
} QTimeRange;

QTimeRange QTimeRangeMake(int64_t startPoint, int64_t endPoint);
bool QTimeRangeIsVaild(QTimeRange qrange);
int64_t QTimeRangeGetLenght(QTimeRange qrange);

int64_t QMediaRangeGetLenght(QMediaRange* qrange);

NSString * QGetMediaSDKVersion();
    
#ifdef __cplusplus
}
#endif


@interface QCommonUtils : NSObject

+ (CGFloat)millisecondsToCGFloatSeconds:(int64_t)milliseconds;
+ (int64_t)CGFloatSecondsToMilliseconds:(CGFloat)seconds;

@end

@protocol Serializable <NSObject>
- (NSDictionary*)serialize;
@end

#endif /* QCOMMON_H */
