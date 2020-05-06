//
//  QAssetReaderSource.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaSource.h"

@interface QAssetReaderSource : NSObject<QMediaSource>

- (instancetype)initWithFilePath:(NSString *)filePath;

/**output target
 * MediaSource can adaptive by output target
 */
@property (nonatomic, weak) id<QVideoTarget> videoTarget;
@property (nonatomic, weak) id<QAudioTarget> audioTarget;

//media source describe
@property (nonatomic, readonly) QVideoDescribe* videoDesc;
@property (nonatomic, readonly) QAudioDescribe* audioDesc;

//after load, media describe is ready
- (bool)load;
- (void)unload;
- (bool)isInit;

//these methods called by mediaTrack
- (bool)start:(int64_t)startMSec end:(int64_t)endMSec;
- (void)stop;
- (bool)seekTo:(int64_t)mSec;
- (bool)isEOF;
- (bool)isStarted;
- (int64_t)getMediaDuration;

- (SampleBufferFrame)readNextVideoFrame;
- (SampleBufferFrame)readNextAudioFrame;

@end
