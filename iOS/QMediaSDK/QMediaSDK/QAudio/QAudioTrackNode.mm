//
//  QAudioNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAudioTrackNode_internal.h"
#import "QMediaTrack_internal.h"


@implementation QAudioTrackNode {
    MediaAudioChannelRef _audioNode;
    __weak QMediaTrack *_mediaTrack;
}

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack {
    if (! mediaTrack.native->getAudioDescribe()) //media source doesn't contain audio channel
        return nil;
    
    return [self initWithNode:MediaAudioChannelRef(new MediaAudioChannel(mediaTrack.native.get()))];
}

#pragma mark - private function
- (instancetype)initWithNode:(MediaAudioChannelRef)audioNode;
{
    if ((self = [super init]) != nil) {
        _audioNode = audioNode;
        if (_audioNode == nullptr) {
            return nil;
        }
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

#pragma mark private
- (MediaAudioChannelRef)native {
    return _audioNode;
}

- (void)setMediaTrack:(QMediaTrack*)mediaTrack {
    _mediaTrack = mediaTrack;
}

@end
