//
//  QMediaTrack.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "QInternalVideoSource.h"
#import "QMediaTrack_internal.h"
#import "QGraphicNode_internal.h"
#import "QMediaSource_internal.h"

#include "EffectEditor/MediaTrackImpl.h"

@interface QAudioNode(init)
- (instancetype)initWithAudioChannel:(MediaAudioChannelRef)audioNode;
@end

@implementation QMediaTrack {
    std::shared_ptr<MediaTrack> _mediaTrackNative;
    id<QMediaSource> _mediaSource;
    QGraphicNode* _graphic;
    QAudioNode* _audio;
}

- (instancetype)initWithMediaSource:(id<QMediaSource>)mediaSource
{
    if ((self = [super init]) != nil) {
        _mediaSource = mediaSource;
        _mediaTrackNative = std::shared_ptr<MediaTrack>(new MediaTrackImpl(MediaSourceRef(new MeidaSourceAdapter(mediaSource))));
        if(_mediaTrackNative->prepare()){
            _graphic = [[QGraphicNode alloc] initWithNode:_mediaTrackNative->getMediaGraphicChannel()];
            _audio = [[QAudioNode alloc] initWithAudioChannel:_mediaTrackNative->getMediaAudioChannel()];
        }else
            self = nil;
    }
    return self;
}

- (instancetype)initWithMediaSourceNative:(MediaSourceRef)mediaSource
{
    if ((self = [super init]) != nil) {
        _mediaTrackNative = std::shared_ptr<MediaTrack>(new MediaTrackImpl(mediaSource));
        if(_mediaTrackNative->prepare()){
            _graphic = [[QGraphicNode alloc] initWithNode:_mediaTrackNative->getMediaGraphicChannel()];
            _audio = [[QAudioNode alloc] initWithAudioChannel:_mediaTrackNative->getMediaAudioChannel()];
        }else
            self = nil;
    }
    return self;
}

- (void)dealloc
{
    _mediaTrackNative.reset();
    _mediaSource = nil;
}

- (bool) prepare {
    if(_mediaTrackNative->prepare()){
        return true;
    }
    return false;
}
- (bool) setTimeTo:(int)mSec {
    return _mediaTrackNative->setPositionTo(mSec);
}
- (void) stopMedia {
    _mediaTrackNative->stopMedia();
}

- (bool)isPrepare{
    return _mediaTrackNative->isPrepare();
}
- (float)getPlaySpeed{
    return _mediaTrackNative->getPlaySpeed();
}
- (int64_t)getSourceDuration{
    return _mediaTrackNative->getMediaDuration();
}

- (NSRange)getDisplayTrackRange{
    NSRange nsRange;
    Range<int64_t> range = _mediaTrackNative->getDisplayTrackRange();
    nsRange.location = (NSUInteger)range._start;
    nsRange.length = (NSUInteger)range.getLength();
    return nsRange;
}
- (void)setDisplayTrackRange:(NSRange)range{
    Range<int64_t> absoluteRange((int64_t)range.location,(int64_t)(range.location + range.length));
    _mediaTrackNative->setDisplayTrackRange(absoluteRange);
}
- (void)setSourceRange:(NSRange)range{
    Range<int64_t> selectSourceRange((int64_t)range.location,(int64_t)(range.location + range.length));
    _mediaTrackNative->setSourceRange(selectSourceRange);
}

- (float)timeScale{
    return _mediaTrackNative->getTimeScale();
}
- (void)setTimeScale:(float)timeScale{
    return _mediaTrackNative->setTimeScale(timeScale);
}
- (int)repeatTimes{
    return _mediaTrackNative->getTimeScale();
}
- (void)setRepeatTimes:(int)repeatTimes{
    return _mediaTrackNative->setRepeatTimes(repeatTimes);
}

- (id<QMediaSource>)source{
    return _mediaSource;
}

- (QGraphicNode*)graphic{
    return _graphic;
}

- (QAudioNode*)audio{
    return _audio;
}

- (MediaTrackRef)native
{
    return _mediaTrackNative;
}

@end
