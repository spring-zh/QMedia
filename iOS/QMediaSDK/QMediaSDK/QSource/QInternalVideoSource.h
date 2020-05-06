//
//  QInternalVideoSource.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaDescribe.h"
#include "EffectEditor/MediaSource.h"

@interface QInternalVideoSource : NSObject

- (instancetype)initWithFilePath:(NSString *)filePath;

@property (nonatomic, readonly) QVideoDescribe* videoDesc;
@property (nonatomic, readonly) QAudioDescribe* audioDesc;

@property (nonatomic, readonly) MediaSourceRef native;

@end
