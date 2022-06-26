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
- (void)onPrepared:(nonnull QEditorPlayer*)player;
- (void)onProgressUpdated:(nonnull QEditorPlayer*)player time:(CGFloat)cgfTime;
- (void)onPlayerSeekedTo:(nonnull QEditorPlayer*)player position:(CGFloat)cgfTime;
- (void)onStarted:(nonnull QEditorPlayer*)player error:(int)errcode;
- (void)onStoped:(nonnull QEditorPlayer*)player;
- (void)onCompleted:(nonnull QEditorPlayer*)player error:(int)errcode;
- (void)onEvent:(nonnull QEditorPlayer*)player eventid:(int32_t)eventid msg:(nonnull NSDictionary<NSString *, NSString *> *)msg;
@end

@interface QEditorPlayer : NSObject
- (instancetype _Nullable )initWithQueue:(dispatch_queue_t _Nullable )cbQueue;

@property (nonatomic, readonly) QEditorPlayerState state;
@property (nonatomic, strong) QPlayerView* _Nullable playerView;

@property (nonatomic, readonly) NSArray<QMediaSegment *>* _Nonnull segments;

- (void)addObserver:(id<QEditorPlayerObserver>_Nonnull)observer;
- (void)removeObserver:(id<QEditorPlayerObserver>_Nonnull)observer;
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

- (int)getState;

@end
