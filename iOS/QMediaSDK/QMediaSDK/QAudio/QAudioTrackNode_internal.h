//
//  QAudioTrackNode_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAudioTrackNode.h"
#include "EffectEditor/MediaAudioChannel.h"

@interface QAudioTrackNode(internal)
- (instancetype)initWithNode:(MediaAudioChannelRef)audioNode;
@property (nonatomic, readonly) MediaAudioChannelRef native;

- (void)setMediaTrack:(QMediaTrack*)mediaTrack;
@end
