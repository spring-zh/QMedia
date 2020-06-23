//
//  QFileExporterTarget.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaDescribe.h"
#import "QAudioTarget.h"
#import "QVideoTarget.h"

@interface QFileExporterTarget : NSObject <QAudioTarget, QVideoTarget>

- (instancetype)initWithPath:(NSString*)filePath;
- (bool)startAudio;
- (bool)stopAudio;
- (bool)startVideo;
- (bool)stopVideo;

@property (nonatomic, readonly) bool hasAudio;
@end
