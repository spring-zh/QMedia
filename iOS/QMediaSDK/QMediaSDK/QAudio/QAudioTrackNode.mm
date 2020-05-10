//
//  QAudioNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAudioTrackNode.h"
#import "QMediaTrack_internal.h"
#include "EffectEditor/MediaAudioChannel.h"


@implementation QAudioTrackNode {
    MediaAudioChannelRef _audioNode;
    __weak QMediaTrack *_mediaTrack;
}

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack {
    if (! mediaTrack.native->getAudioDescribe()) //media source doesn't contain audio channel
        return nil;
    if ((self = [super init]) != nil) {
        _mediaTrack = mediaTrack;
        _audioNode = MediaAudioChannelRef(new MediaAudioChannel(mediaTrack.native.get()));
    }
    return self;
}

- (bool)enable{
    return _audioNode->isEnable();
}
- (void)setEnable:(bool)enable{
    _audioNode->setEnable(enable);
}
- (float)volume{
    return _audioNode->getVolume();
}
- (void)setVolume:(float)volume{
    _audioNode->setVolume(volume);
}

@end
