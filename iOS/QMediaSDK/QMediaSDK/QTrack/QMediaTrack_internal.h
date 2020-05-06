//
//  QMediaTrack_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectEditor/MediaTrack.h"
#import "QMediaTrack.h"

@interface QMediaTrack(internal)
- (instancetype)initWithMediaSourceNative:(MediaSourceRef)mediaSource;
@property (nonatomic, readonly) MediaTrackRef native;
@end
