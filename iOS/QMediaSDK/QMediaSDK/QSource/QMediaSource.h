//
//  QMediaSource.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "QMediaDescribe.h"
#import "QVideoTarget.h"
#import "QAudioTarget.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SampleBufferFrame {
    int error;//error code: 0 success, -1 system error, 1 read end
    bool invalid_pts; //set true when use live stream
    CMSampleBufferRef sampleBuffer;
}SampleBufferFrame;

#ifdef __cplusplus
}
#endif

@protocol QMediaSource <NSObject>
@required
//output target
@property (nonatomic, weak) id<QVideoTarget> videoTarget;
@property (nonatomic, weak) id<QAudioTarget> audioTarget;

//media source describe
@property (nonatomic, readonly) QVideoDescribe* videoDesc;
@property (nonatomic, readonly) QAudioDescribe* audioDesc;

//TODO: please use these video format
//kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange , kCVPixelFormatType_420YpCbCr8BiPlanarFullRange ,kCVPixelFormatType_32BGRA
@property (nonatomic) uint32_t video_frame_format;

- (bool)load;
- (void)unload;
- (bool)isInit;

- (bool)start:(int64_t)startMSec end:(int64_t)endMSec;
- (void)stop;
- (bool)seekTo:(int64_t)mSec;
- (bool)isEOF;
- (bool)isStarted;
- (int64_t)getMediaDuration;

- (SampleBufferFrame)readNextVideoFrame;
- (SampleBufferFrame)readNextAudioFrame;

@end 
