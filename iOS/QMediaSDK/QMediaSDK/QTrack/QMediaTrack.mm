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

@implementation QMediaTrack {
    std::shared_ptr<MediaTrack> _mediaTrackNative;
    id<QMediaSource> _mediaSource;
    NSString *_uid;
    QVideoTrackNode* _graphic;
    QAudioTrackNode* _audio;
}

- (instancetype)initWithMediaSource:(id<QMediaSource>)mediaSource
{
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initWithMediaSource:mediaSource uid:(__bridge_transfer NSString *)uuid];
}

- (instancetype)initWithMediaSource:(id<QMediaSource>)mediaSource uid:(NSString*)uid
{
    if ((self = [super init]) != nil) {
        _uid = [uid copy];
        _mediaSource = mediaSource;
        _mediaTrackNative = MediaTrackRef(new MediaTrackImpl(MediaSourceRef(new MediaSourceAdapter(mediaSource))));
        if(_mediaTrackNative->prepare()){
//            _graphic = [[QVideoTrackNode alloc] initFromTrack:self];
//            _audio = [[QAudioTrackNode alloc] initFromTrack:self];
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
//            _graphic = [[QVideoTrackNode alloc] initFromTrack:self];
//            _audio = [[QAudioTrackNode alloc] initFromTrack:self];
        }else
            self = nil;
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"%@ name(%@) dealloc ", self, self.displayName);
    _mediaTrackNative.reset();
    _mediaSource = nil;
}

- (bool)generateAudioTrackNode:(QCombiner*)combiner {
    if (_mediaSource.audioDesc) {
        _audio = [[QAudioTrackNode alloc] initFromTrack:self combiner:combiner];
    }
    return _audio != nil;
}
- (bool)generateVideoTrackNode:(QCombiner*)combiner {
    if (_mediaSource.videoDesc) {
        _graphic = [[QVideoTrackNode alloc] initFromTrack:self combiner:combiner];
    }
    return _graphic != nil;
}

- (bool)generateAudioTrackNode:(QCombiner*)combiner uid:(NSString*)uid{
    if (_mediaSource.audioDesc) {
        _audio = [[QAudioTrackNode alloc] initFromTrack:self combiner:combiner uid:uid];
    }
    return _audio != nil;
}
- (bool)generateVideoTrackNode:(QCombiner*)combiner uid:(NSString*)uid{
    if (_mediaSource.videoDesc) {
        _graphic = [[QVideoTrackNode alloc] initFromTrack:self combiner:combiner uid:uid];
    }
    return _graphic != nil;
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

- (NSString*)uid {
    return _uid;
}

- (QVideoDescribe *)videoDesc {
    return _mediaSource.videoDesc;
}

- (QAudioDescribe *)audioDesc {
    return _mediaSource.audioDesc;
}

- (QTimeRange)sourceRange{
    auto range = _mediaTrackNative->getSourceRange();
    QTimeRange qRange = {range._start, range._end};
    return qRange;
}

- (void)setDisplayRange:(QTimeRange)range{
    Range<int64_t> displayRange(range.startPoint,range.endPoint);
    _mediaTrackNative->setDisplayTrackRange(displayRange);
}

- (QTimeRange)displayRange{
    auto range = _mediaTrackNative->getDisplayTrackRange();
    QTimeRange qRange = {range._start, range._end};
    return qRange;
}

- (void)setSourceRange:(QTimeRange)range{
    Range<int64_t> selectSourceRange(range.startPoint,range.endPoint);
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

- (QVideoTrackNode*)graphic{
    return _graphic;
}

- (QAudioTrackNode*)audio{
    return _audio;
}

@synthesize displayName = _displayName;
- (NSString *)displayName {
    return _displayName;
}

- (void)setDisplayName:(NSString*)displayName {
    _displayName = [displayName copy];
}

- (MediaTrackRef)native
{
    return _mediaTrackNative;
}

@end
