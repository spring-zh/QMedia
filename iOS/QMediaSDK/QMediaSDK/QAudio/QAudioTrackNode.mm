//
//  QAudioNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAudioTrackNode_internal.h"
#import "QMediaTrack_internal.h"
#import "QCombiner_internal.h"


@implementation QAudioTrackNode {
    NSString *_uid;
    MediaAudioChannelRef _audioNode;
    __weak QMediaTrack *_mediaTrack;
    __weak QCombiner* _combiner;
}

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner {
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initFromTrack:mediaTrack combiner:combiner uid:(__bridge_transfer NSString *)uuid];
}
- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner uid:(NSString*)uid {
    if (! mediaTrack.native->getAudioDescribe()) //media source doesn't contain audio channel
        return nil;
    _uid = uid;
    _combiner = combiner;
    if([self initWithNode:MediaAudioChannelRef(new MediaAudioChannel(mediaTrack.native.get()))]) {
        [combiner attachAudioNode:self parent:nil];
    }
    return self;
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

- (NSString*)uid {
    return _uid;
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
