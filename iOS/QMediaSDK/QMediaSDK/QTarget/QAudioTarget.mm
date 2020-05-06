//
//  QAudioTarget.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAudioTarget_internal.h"
#import <AVFoundation/AVFoundation.h>

#include <memory>

@implementation QAudioTarget{
    std::unique_ptr<AudioTargetAdapter> _nativeTarget;
    id<QAudioTarget> _ocTarget;
}

-(instancetype)initWithTarget:(id<QAudioTarget>)target {
    self = [super init];
    if (self) {
        _ocTarget = target;
        target.audioRender = self;
        _nativeTarget = std::unique_ptr<AudioTargetAdapter>(new AudioTargetAdapter(target));
    }
    return self;
}

- (void)dealloc
{
    _ocTarget = nil;
}

-(id<QAudioTarget>)ocTarget{
    return _ocTarget;
}

-(AudioTarget*)native{
    return _nativeTarget.get();
}

#pragma mask implementation AudioRender
- (bool)onAudioRender:(uint8_t * const)buffer needBytes:(unsigned)needBytes  wantTime:(int64_t)wantTime
{
    return _nativeTarget->onAudioDataRequest(buffer, needBytes, wantTime);
}

@end
