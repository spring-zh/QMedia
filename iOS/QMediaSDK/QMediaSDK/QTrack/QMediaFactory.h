//
//  QMediaFactory.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "QMediaTrack.h"

@class QCombiner;

@interface QMediaFactory : NSObject

@property (nonatomic, readonly) id<QVideoTarget> videoTarget;
@property (nonatomic, readonly) id<QAudioTarget> audioTarget;

- (QMediaTrack*)createVideoTrack:(NSString *)filePath combiner:(QCombiner*)combiner;
- (QMediaTrack*)createAudioTrack:(NSString *)filePath combiner:(QCombiner*)combiner;
- (QMediaTrack*)createCaptureTrack:(AVCaptureSessionPreset)preset position:(AVCaptureDevicePosition)position video:(bool)enableVideo audio:(bool)enableAudio combiner:(QCombiner*)combiner;

- (QMediaTrack*)createOldVideoTrack:(NSString *)filePath combiner:(QCombiner*)combiner DEPRECATED_ATTRIBUTE;

@end
