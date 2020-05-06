//
//  QEditorPlayer.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "QGraphicNode.h"
#import "QMediaFactory.h"

@class QEditorPlayer;
@class QObserverHoster;

typedef NS_ENUM(NSInteger, QEditorPlayerState) {
    XMPlayerState_Idle = 0,
    XMPlayerState_Ready,
    XMPlayerState_Playing,
    XMPlayerState_Paused,
    XMPlayerState_Finished,
    XMPlayerState_Stopped,
    XMPlayerState_Error
};

@protocol QEditorPlayerObserver <NSObject>
@optional
- (void)onPrepare;
- (void)onPlayerChangedFromOldState:(NSNumber*)xmsOldState
                         toNewState:(NSNumber*)xmsNewState;
- (void)onPlayerTimeProgressUpdated:(NSNumber*)cgfTime;
- (void)onPlayerSeekedTo:(NSNumber*)cgfTime;
- (void)onCompleted;
@end

@interface QEditorPlayer : NSObject <QVideoRender ,QAudioRender>
- (instancetype)initWithQueue:(dispatch_queue_t)cbQueue;
@property (nonatomic, readonly) NSArray<QMediaTrack*>* subObjects;

@property (nonatomic, readonly) QEditorPlayerState state;
@property (nonatomic, readonly) int64_t position;
@property (nonatomic, readonly) BOOL isPaused;
@property (nonatomic) BOOL loopPlay;
@property (nonatomic, strong) id<QVideoTarget> playerView;

@property (nonatomic, readonly) QGraphicNode* rootNode;

//mediaFactory effective after set playerView
@property (nonatomic, readonly) QMediaFactory* mediaFactory;

- (void)addObserver:(id<QEditorPlayerObserver>)observer;
- (void)removeObserver:(id<QEditorPlayerObserver>)observer;
- (void)removeAllObservers;

- (void)addMediaTrack:(QMediaTrack*)track;
- (void)removeMediaTrack:(QMediaTrack*)track;

- (BOOL)start;
- (BOOL)play;
- (BOOL)pause;
- (BOOL)stop;
- (BOOL)seekTo:(CGFloat)timePoint :(int)flags;

- (NSRange)getMediaTimeRange;

@end
