//
//  QCommon.m
//  QMediaSDK
//
//  Created by LINQIUMING on 05/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCommon.h"
//#import "Version.h"


QVector QVectorV1(float v0){
    QVector value;
    value.v0 = v0;
    return value;
}
QVector QVectorV2(float v0, float v1) {
    QVector value;
    value.v0 = v0;
    value.v1 = v1;
    return value;
}
QVector QVectorV3(float v0, float v1, float v2) {
    QVector value;
    value.v0 = v0;
    value.v1 = v1;
    value.v2 = v2;
    return value;
}
QVector QVectorV4(float v0, float v1, float v2, float v3) {
    QVector value;
    value.v0 = v0;
    value.v1 = v1;
    value.v2 = v2;
    value.v3 = v3;
    return value;
}

QTimeRange QTimeRangeMake(int64_t startPoint, int64_t endPoint)
{
    QTimeRange range = {startPoint, endPoint};
    return range;
}
bool QTimeRangeIsVaild(QTimeRange qrange)
{
    return QTimeRangeGetLenght(qrange) > 0;
}

int64_t QTimeRangeGetLenght(QTimeRange qrange)
{
    return qrange.endPoint - qrange.startPoint;
}

int64_t QMediaRangeGetLenght(QMediaRange* qrange)
{
    return qrange.end - qrange.start;
}

#define XM_SDK_VERSION "1.3"
NSString * QGetMediaSDKVersion(){
    char s[16];
//    sprintf(s, "%s%s",XM_SDK_VERSION,GET_MEDIA_CORE_VERSION());
    
    NSString * ver = [[NSString alloc] initWithUTF8String:s];
    return ver;
}

@implementation QCommonUtils

+ (CGFloat)millisecondsToCGFloatSeconds:(int64_t)milliseconds
{
    return ((CGFloat)milliseconds)/1000;
}

+ (int64_t)CGFloatSecondsToMilliseconds:(CGFloat)seconds
{
    return (int64_t)(seconds*1000);
}

@end
