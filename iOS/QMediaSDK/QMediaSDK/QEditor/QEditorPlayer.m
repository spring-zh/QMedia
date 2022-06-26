//
//  QEditorPlayer.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//


#import "QEditorPlayer.h"
#import "QCommon.h"
#import "QObserverHoster.h"
#import "QAudioPlayer.h"
#import "QMediaEngine.h"
#import "QEditorPlayerCallback.h"
#import "QEditorPlayerInternal.h"
#import "QMediaSessionInternal.h"

@interface QEditorPlayer () <QEditorPlayerCallback>
@property (nonatomic) QObserverHoster* observerHoster;
@property (nonatomic, readonly) dispatch_queue_t cbQueue;
@end

#pragma mark - implementation XMEEPlayer

@implementation QEditorPlayer {
    QPlayerView* _player_view;
    QEditorPlayerInternal* _internal;
    QMediaSessionInternal* _session;
    QAudioPlayer * _audio_player;
    QVideoRender * _video_render;
    QAudioRender * _audio_render;
}

@synthesize observerHoster = _observerHoster;
@synthesize cbQueue = _cbQueue;

- (instancetype)init
{
    return [self initWithQueue:dispatch_get_main_queue()];
}

- (instancetype)initWithQueue:(dispatch_queue_t)cbQueue
{
    if ((self = [super init]) != nil) {
        _observerHoster = [[QObserverHoster alloc] init];
        _internal = [QMediaEngine createEditorPlayerInternal];
        _session = [_internal getSession];
        _video_render = [_session getVideoRender];
        _audio_render = [_session getAudioRender];
        _audio_player = [[QAudioPlayer alloc] init];
        [_audio_player setAudioRender:_audio_render];
        [_session setAudioRunLoop:_audio_player];
        _cbQueue = cbQueue;
        //FIXME: call audioplayer init in session
        [_audio_player audioInit:[QMediaEngine audioOut]];
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"%@ dealloc ", self);
}

- (QPlayerView*)playerView {
    return _player_view;
}

- (void)setPlayerView:(QPlayerView*)playerView{
    [playerView setVideoRender:_video_render];
    [_session setVideoRunLoop:playerView];
    _player_view = playerView;
}

- (QEditorPlayerState)state {
    return (QEditorPlayerState)[_internal getState];
}

- (NSArray<QMediaSegment *>*)segments {
    return [_session getSegments];
}

#pragma mark - Observer Logic
- (void)addObserver:(id<QEditorPlayerObserver>)observer
{
    [_observerHoster addObserver:observer];
}

- (void)removeObserver:(id<QEditorPlayerObserver>)observer
{
    [_observerHoster removeObserver:observer];
}

- (void)removeAllObservers
{
    [_observerHoster removeAllObservers];
}

- (nullable QMediaSegment *)cresteMediaSegment:(nonnull NSString *)filename
                                          flag:(int32_t)flag {
    NSLog(@"QEditorPlayer cresteMediaSegment in");
    return [_session cresteMediaSegment:filename flag:flag];
}

- (BOOL)addMediaSegment:(nullable QMediaSegment *)segment {
    NSLog(@"QEditorPlayer addMediaSegment in");
    return [_session addMediaSegment:segment];
}

- (BOOL)removeMediaSegment:(nullable QMediaSegment *)segment {
    NSLog(@"QEditorPlayer removeMediaSegment in");
    return [_session removeMediaSegment:segment];
}

- (nonnull QMediaRange *)getTotalTimeRange {
    return [_session getTotalTimeRange];
}

- (void)setDisplayLayerSize:(nonnull QSize *)size {
    [_session setDisplayLayerSize:size];
}

- (nonnull QSize *)getDisplayLayerSize {
    return [_session getDisplayLayerSize];
}

- (void)setBkColor:(nonnull QVec4f *)color {
    [_session setBkColor:color];
}

- (nonnull QVec4f *)getBkColor {
    return [_session getBkColor];
}

#pragma mark Player Control
- (void)start {
    NSLog(@"QEditorPlayer start in");
    [_internal setCallback:self];
    [_session prepare];
}

- (void)play {
    NSLog(@"QEditorPlayer play in");
    [_internal play];
}

- (void)pause {
    [_internal pause];
}

- (void)stop {
    [_session stop];
}

- (void)seek:(int64_t)timeMs
        flag:(int32_t)flag {
    [_internal seek:timeMs flag:flag];
}

- (BOOL)isUserPaused {
    return [_internal isUserPaused];
}

- (int)getState {
    return [_internal getState];
}

- (int64_t)getPosition {
    return [_internal getPosition];
}


#pragma mark --------implemention QEditorPlayerCallback ---------

- (void)onPrepare:(int32_t)code {
    NSLog(@"QEditorPlayer onPrepare");
    dispatch_async(_cbQueue, ^{
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onPrepare:) withObjects:self];
        }
    });
}

- (void)onStarted:(int32_t)code {
    NSLog(@"QEditorPlayer onStarted code:%d", code);
    dispatch_async(_cbQueue, ^{
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onStarted:error:) withObjects:self, code, nil];
        }
    });
}

- (void)onStoped:(int32_t)code {
    NSLog(@"QEditorPlayer onStoped code:%d", code);
    dispatch_async(_cbQueue, ^{
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onStoped:) withObjects:self, nil];
            [_internal setCallback:nil];
        }
    });
}

- (void)onSeekTo:(int64_t)timeMs {
//    NSNumber* time_ms = @(timeMs);
    dispatch_async(_cbQueue, ^{
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onPlayerSeekedTo:position:) withObjects:self, timeMs, nil];
        }
    });
}

- (void)onProgressUpdated:(int64_t)timeMs {
    dispatch_async(_cbQueue, ^{
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onProgressUpdated:time:) withObjects:self, @((CGFloat)timeMs/1000), nil];
        }
    });
}

- (void)onCompleted:(int)code {
    NSLog(@"QEditorPlayer onCompleted code:%d", code);
    dispatch_async(_cbQueue, ^{
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onCompleted:error:) withObjects:self, code, nil];
        }
    });
}

- (void)onEvent:(int32_t)eventid
            msg:(nonnull NSDictionary<NSString *, NSString *> *)msg {
    dispatch_async(_cbQueue, ^{
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onEvent:eventid:msg:) withObjects:self, eventid, msg, nil];
        }
    });
}

@end
