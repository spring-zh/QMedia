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
    QVideoTrackNode* _graphic;
    QAudioTrackNode* _audio;
}

- (instancetype)initWithMediaSource:(id<QMediaSource>)mediaSource
{
    if ((self = [super init]) != nil) {
        _mediaSource = mediaSource;
        _mediaTrackNative = std::shared_ptr<MediaTrack>(new MediaTrackImpl(MediaSourceRef(new MediaSourceAdapter(mediaSource))));
        if(_mediaTrackNative->prepare()){
            _graphic = [[QVideoTrackNode alloc] initFromTrack:self];
            _audio = [[QAudioTrackNode alloc] initFromTrack:self];
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
            _graphic = [[QVideoTrackNode alloc] initFromTrack:self];
            _audio = [[QAudioTrackNode alloc] initFromTrack:self];
        }else
            self = nil;
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"%@ dealloc ", self);
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

- (NSRange)sourceRange{
    auto range = _mediaTrackNative->getSourceRange();
    NSRange nsRange;
    nsRange.location = (NSUInteger)range._start;
    nsRange.length = (NSUInteger)range.getLength();
    return nsRange;
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

//- (QGraphicNode*)graphic{
//    return _graphic;
//}

- (QAudioTrackNode*)audio{
    return _audio;
}

- (MediaTrackRef)native
{
    return _mediaTrackNative;
}

- (NSDictionary*)serialize {
    NSDictionary* dic = [[NSMutableDictionary alloc] init];
    [dic setValue:NSStringFromClass(self.class) forKey:@"objectType"];
    [dic setValue:@([self timeScale]) forKey:@"timeScale"];
    [dic setValue:@([self repeatTimes]) forKey:@"repeatTimes"];
    {
        NSRange sRange = [self sourceRange];
        NSDictionary* dic_sourceRange = [[NSMutableDictionary alloc] init];
        [dic_sourceRange setValue:@(sRange.location) forKey:@"location"];
        [dic_sourceRange setValue:@(sRange.length) forKey:@"length"];
        [dic setValue:dic_sourceRange forKey:@"sourceRange"];
    }
    {
        NSRange dRange = [self getDisplayTrackRange];
        NSDictionary* dic_displayTrackRange = [[NSMutableDictionary alloc] init];
        [dic_displayTrackRange setValue:@(dRange.location) forKey:@"location"];
        [dic_displayTrackRange setValue:@(dRange.length) forKey:@"length"];
        [dic setValue:dic_displayTrackRange forKey:@"displayTrackRange"];
    }
       
    [dic setValue:[_mediaSource serialize] forKey:@"mediaSource"];

    return dic;
}

@end
