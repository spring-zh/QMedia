//
//  QStreamInfoForMerge.h
//  QMediaSDK
//
//  Created by zhen zhang on 2017/5/2.
//  Copyright © 2017年 QMedia. All rights reserved.
//

#ifndef XMSourceFileForMerge_h
#define XMSourceFileForMerge_h

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

typedef struct QStreamInfoForMerge
{
    BOOL enable; // 是否要添加该流
    BOOL isAbsTime; // 是否绝对时间，YES：应该取destStartTime，NO：应该取offsetFromLastStream
    union
    {
        CMTime destStartTime; // 合并到目标文件的时间
        CMTime offsetFromLastStreamFile; // 相对前一个被合并文件相同流的时间偏移
    };
    CMTimeRange sourceTimeRange; // 要合并的视频段在该流中的时间区间
} XMStreamInfoForMerge;

@interface QSourceFileForMerge : NSObject
@property NSString* path; // 文件路径
@property(readonly) NSURL* url; // 文件URL，和path指向同一个文件，表示格式不同而已
@property XMStreamInfoForMerge* videoStreamInfo; // 视频流
@property XMStreamInfoForMerge* audioStreamInfo; // 音频流
-(instancetype)init;
@end

#endif /* XMSourceFileForMerge_h */
