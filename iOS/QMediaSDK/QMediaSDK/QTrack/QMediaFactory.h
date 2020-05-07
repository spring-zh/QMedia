//
//  QMediaFactory.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "QMediaTrack.h"

@interface QMediaFactory : NSObject

@property (nonatomic, readonly) id<QVideoTarget> videoTarget;
@property (nonatomic, readonly) id<QAudioTarget> audioTarget;

- (QMediaTrack*)createVideoTrack:(NSString *)filePath;
- (QMediaTrack*)createCaptureTrack:(AVCaptureSessionPreset)preset position:(AVCaptureDevicePosition)position;

- (QMediaTrack*)createOldVideoTrack:(NSString *)filePath DEPRECATED_ATTRIBUTE;

@end
