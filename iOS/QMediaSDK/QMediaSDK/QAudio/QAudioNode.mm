//
//  QAudioNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAudioNode.h"
#include "EffectEditor/MediaAudioChannel.h"


@implementation QAudioNode {
    MediaAudioChannelRef _audioNode;
}

- (instancetype)initWithAudioChannel:(MediaAudioChannelRef)audioNode
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

@end
