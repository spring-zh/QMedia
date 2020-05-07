//
//  QCombiner.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCombiner_internal.h"
#import "QCommon.h"
#import "QObserverHoster.h"
#import "QAudioTarget_internal.h"
#import "QVideoTarget_internal.h"
#import "QGraphicNode_internal.h"
#import "QMediaTrack_internal.h"
#import "QMediaFactory_internal.h"

extern const struct VideoDescribe XMToVideoDescribe(QVideoDescribe* xmdesc);
extern const struct AudioDescribe XMToAudioDescribe(QAudioDescribe* xmdesc);

@implementation QCombiner {
    NSMutableArray<QMediaTrack*>* _subObjectArray;
    EffectCombiner* _combiner;
    QMediaFactory *_mediaFactory;
    QGraphicNode* _rootNode;
}

- (instancetype)initWithNative:(EffectCombiner* )combiner_native
{
    if ((self = [super init]) != nil) {
        _combiner = combiner_native;
        _mediaFactory = [[QMediaFactory alloc] init];
        _subObjectArray = [NSMutableArray new];
        _rootNode = [[QGraphicNode alloc] initWithNode:_combiner->getRootNode()];
    }
    return self;
}

- (EffectCombiner*)native {
    return _combiner;
}

- (id<QVideoTarget>)videoTarget {
    return _mediaFactory.videoTarget;
}

- (id<QAudioTarget>)audioTarget {
    return _mediaFactory.audioTarget;
}

- (void)setVideoTarget:(id<QVideoTarget>)videoTarget {
    [_mediaFactory setVideoTarget:videoTarget];
    _combiner->setVideoTarget([_mediaFactory nativeVideoTarget]);
}

- (void)setAudioTarget:(id<QAudioTarget>)audioTarget {
    [_mediaFactory setAudioTarget:audioTarget];
    _combiner->setAudioTarget([_mediaFactory nativeAudioTarget]);
}

- (void)setVideoConfig:(QVideoDescribe*)describe
{
    _combiner->setvideoConfig(XMToVideoDescribe(describe));
}
- (void)setAudioConfig:(QAudioDescribe*)describe
{
    _combiner->setAudioConfig(XMToAudioDescribe(describe));
}

- (NSArray<QMediaTrack*>*)subObjects
{
    return _subObjectArray;
}

#pragma mark - Player Control
- (void)addMediaTrack:(QMediaTrack*)track
{
    _combiner->addMediaTrack(track.native);
    [_subObjectArray addObject:track];
}
- (void)removeMediaTrack:(QMediaTrack*)track
{
    _combiner->removeMediaTrack(track.native);
    [_subObjectArray removeObject:track];
}

- (NSRange)mediaTimeRange
{
    NSRange nsRange;
    Range<int64_t> tRange = _combiner->getMediaTimeRange();
    nsRange.location = tRange._start;
    nsRange.length = tRange.getLength();
    return nsRange;
}

- (QGraphicNode*)rootNode {
    return _rootNode;
}

- (QMediaFactory*)mediaFactory {
//    static XMMediaFactory* instance = nil;
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        instance = [[XMMediaFactory alloc] initWithTarget:_videoTarget audio:_audioTarget];
//    });
//    return instance;
    return _mediaFactory;
}

@end
