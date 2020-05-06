//
//  QVideoTarget.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QVideoTarget_internal.h"
#import <AVFoundation/AVFoundation.h>
#include <memory>

@implementation QVideoTarget{
    std::unique_ptr<VideoTargetAdapter> _nativeTarget;
    id<QVideoTarget> _ocTarget;
}

-(instancetype)initWithTarget:(id<QVideoTarget>)target {
    self = [super init];
    if (self) {
        _ocTarget = target;
        target.videoRender = self;
        _nativeTarget = std::unique_ptr<VideoTargetAdapter>(new VideoTargetAdapter(target));
    }
    return self;
}

- (void)dealloc
{
    _ocTarget = nil;
}

-(id<QVideoTarget>)ocTarget{
    return _ocTarget;
}

-(VideoTarget*)native{
    return _nativeTarget.get();
}

#pragma mask implementation VideoRender
- (bool)onVideoRender:(int64_t)wantTime {
    return _nativeTarget->onVideoRender(wantTime);
}
- (bool)onVideoCreate {
    return _nativeTarget->onRenderCreate();
}
- (void)onVideoDestroy {
    _nativeTarget->onRenderDestroy();
}

@end
