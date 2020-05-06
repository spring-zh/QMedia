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

//+ (instancetype)sharedInstance;

- (QMediaTrack*)createVideoTrack:(NSString *)filePath;
- (QMediaTrack*)createCaptureTrack:(AVCaptureSessionPreset)preset position:(AVCaptureDevicePosition)position;

- (QMediaTrack*)createOldVideoTrack:(NSString *)filePath DEPRECATED_ATTRIBUTE;

@end
