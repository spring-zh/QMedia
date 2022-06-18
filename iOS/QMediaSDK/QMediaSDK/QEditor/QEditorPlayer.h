//
//  QEditorPlayer.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QSize.h"
#import "QVec4f.h"
#import "QMediaRange.h"
#import "QMediaSegment.h"
#import "QVideoRunloop.h"
#import "QPlayerView.h"

typedef NS_ENUM(NSInteger, QEditorPlayerState) {
    XMPlayerState_Idle = 0,
    XMPlayerState_Ready,
    XMPlayerState_Playing,
    XMPlayerState_Paused,
    XMPlayerState_Finished,
    XMPlayerState_Stopped,
    XMPlayerState_Error
};

@class QEditorPlayer;

@protocol QEditorPlayerObserver <NSObject>
@optional
- (void)onPrepare;
- (void)onPlayerChangedFromOldState:(nonnull QEditorPlayer*)player oldState:(int)xmsOldState
                         toNewState:(int)xmsNewState;
- (void)onPlayerTimeProgressUpdated:(NSNumber*)cgfTime;
- (void)onPlayerSeekedTo:(nonnull QEditorPlayer*)player position:(int64_t)cgfTime;
- (void)onStarted:(nonnull QEditorPlayer*)player;
- (void)onStoped:(nonnull QEditorPlayer*)player;
- (void)onCompleted:(nonnull QEditorPlayer*)player;
@end


@interface QEditorPlayer : NSObject
- (instancetype)initWithQueue:(dispatch_queue_t)cbQueue;

@property (nonatomic, readonly) QEditorPlayerState state;
@property (nonatomic, strong) QPlayerView* _Nullable playerView;

- (void)addObserver:(id<QEditorPlayerObserver>)observer;
- (void)removeObserver:(id<QEditorPlayerObserver>)observer;
- (void)removeAllObservers;

- (nullable QMediaSegment *)cresteMediaSegment:(nonnull NSString *)filename
                                          flag:(int32_t)flag;

- (BOOL)addMediaSegment:(nullable QMediaSegment *)segment;

- (BOOL)removeMediaSegment:(nullable QMediaSegment *)segment;

- (nonnull QMediaRange *)getTotalTimeRange;

- (void)setDisplayLayerSize:(nonnull QSize *)size;

- (nonnull QSize *)getDisplayLayerSize;

- (void)setBkColor:(nonnull QVec4f *)color;

- (nonnull QVec4f *)getBkColor;

/** control */

- (void)start;

- (void)stop;

- (int64_t)getPosition;

- (void)play;

- (void)pause;

- (void)seek:(int64_t)timeMs
        flag:(int32_t)flag;

- (BOOL)isUserPaused;

@end
