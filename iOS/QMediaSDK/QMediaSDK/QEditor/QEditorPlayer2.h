//
//  QEditorPlayer.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCombiner.h"

typedef NS_ENUM(NSInteger, QEditorPlayerState2) {
    XMPlayerState_Idle = 0,
    XMPlayerState_Ready,
    XMPlayerState_Playing,
    XMPlayerState_Paused,
    XMPlayerState_Finished,
    XMPlayerState_Stopped,
    XMPlayerState_Error
};

@protocol QEditorPlayerObserver2 <NSObject>
@optional
- (void)onPrepare;
- (void)onPlayerChangedFromOldState:(NSNumber*)xmsOldState
                         toNewState:(NSNumber*)xmsNewState;
- (void)onPlayerTimeProgressUpdated:(NSNumber*)cgfTime;
- (void)onPlayerSeekedTo:(NSNumber*)cgfTime;
- (void)onCompleted;
@end


@interface QEditorPlayer2 : QCombiner <QVideoRender2 ,QAudioRender2>
- (instancetype)initWithQueue:(dispatch_queue_t)cbQueue;

@property (nonatomic, readonly) QEditorPlayerState2 state;
@property (nonatomic, readonly) int64_t position;
@property (nonatomic, readonly) BOOL isPaused;
@property (nonatomic) BOOL loopPlay;
@property (nonatomic, strong) id<QVideoTarget> playerView;

- (void)addObserver:(id<QEditorPlayerObserver2>)observer;
- (void)removeObserver:(id<QEditorPlayerObserver2>)observer;
- (void)removeAllObservers;

- (BOOL)start;
- (BOOL)play;
- (BOOL)pause;
- (BOOL)stop;
- (BOOL)seekTo:(CGFloat)timePoint :(int)flags;

@end
