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
#import "QAudioTarget_internal.h"
#import "QVideoTarget_internal.h"
#import "QGraphicNode_internal.h"
#import "QMediaTrack_internal.h"
#import "QMediaFactory_internal.h"
#import "QCombiner_internal.h"
#include "EffectEditor/EditorPlayer.h"

@interface QEditorPlayer ()
@property (nonatomic) QObserverHoster* observerHoster;
@property (nonatomic, readonly) dispatch_queue_t cbQueue;
@end

static QEditorPlayerState coreStateToXMState(PlayerState state)
{
    QEditorPlayerState xmState = XMPlayerState_Idle;
    switch (state) {
        case PlayerState::Idle:
        case PlayerState::Initialized:
        case PlayerState::AsyncPreparing:
            xmState = XMPlayerState_Idle;
            break;
            
        case PlayerState::Prepared:
            xmState = XMPlayerState_Ready;
            break;
            
        case PlayerState::Started:
            xmState = XMPlayerState_Playing;
            break;
            
        case PlayerState::Paused:
            xmState = XMPlayerState_Paused;
            break;
            
        case PlayerState::Completed:
            xmState = XMPlayerState_Finished;
            break;
            
        case PlayerState::Stopped:
        case PlayerState::Ended:
            xmState = XMPlayerState_Stopped;
            break;
            
        case PlayerState::Error:
            xmState = XMPlayerState_Error;
            break;
            
        default:
            break;
    }
    return xmState;
}

#pragma mark - Core Player Callback
class CallBackImpl: public EditorPlayer::Callback
{
public:
    CallBackImpl(QEditorPlayer* xmPlayer)
    {
        _xmPlayer = xmPlayer;
    }
    void onPrepare(EditorPlayer::RetCode code) override {
        dispatch_async(_xmPlayer.cbQueue, ^(){
            @autoreleasepool{
                [_xmPlayer.observerHoster notifyObservers:@selector(onPrepare)];
            }
        });
    }
    void onPlayerStateChanged(PlayerState newState, PlayerState oldState) override
    {
        dispatch_async(_xmPlayer.cbQueue, ^(){
            @autoreleasepool{
               QEditorPlayerState xmNewState = coreStateToXMState(newState);
               QEditorPlayerState xmOldState = coreStateToXMState(oldState);
               [_xmPlayer.observerHoster notifyObservers:@selector(onPlayerChangedFromOldState:toNewState:)
                                             withObjects:@(xmOldState), @(xmNewState), nil];
           }
        });
    }
    void onProgressUpdated(int64_t mSec) override
    {
        dispatch_async(_xmPlayer.cbQueue, ^(){
            @autoreleasepool{
            [_xmPlayer.observerHoster notifyObservers:@selector(onPlayerTimeProgressUpdated:)
                                           withObject:@([QCommonUtils millisecondsToCGFloatSeconds:mSec])];
            }
        });
        
    }
    void onSeekTo(int64_t mSec) override
    {
        dispatch_async(_xmPlayer.cbQueue, ^(){
            @autoreleasepool{
                [_xmPlayer.observerHoster notifyObservers:@selector(onPlayerSeekedTo:)
                                               withObject:@([QCommonUtils millisecondsToCGFloatSeconds:mSec])];
            }
        });
        
    }
    void onCompleted() override
    {
        dispatch_async(_xmPlayer.cbQueue, ^(){
            @autoreleasepool{
                [_xmPlayer.observerHoster notifyObservers:@selector(onCompleted)];
            }
        });
    }
private:
    __weak QEditorPlayer* _xmPlayer;
};

#pragma mark - implementation XMEEPlayer

@implementation QEditorPlayer {
    std::unique_ptr<EditorPlayer> _pPlayer;
    std::unique_ptr<CallBackImpl> _playerCallback;
    dispatch_queue_t _cbQueue;
}

@synthesize observerHoster = _observerHoster;
@synthesize cbQueue = _cbQueue;

- (instancetype)init
{
    return [self initWithQueue:dispatch_get_main_queue()];
}

- (instancetype)initWithQueue:(dispatch_queue_t)cbQueue
{
    _pPlayer = std::unique_ptr<EditorPlayer>(new EditorPlayer());
    
    if ((self = [super initWithNative:_pPlayer.get()]) != nil) {
        _observerHoster = [[QObserverHoster alloc] init];
        _playerCallback = std::unique_ptr<CallBackImpl>(new CallBackImpl(self));
        _pPlayer->setCallBack(_playerCallback.get());
        //set audio target
        id<QAudioTarget> audioTarget = [[QAudioPlayer alloc] init];
        [audioTarget setAudioRender:self];
        super.audioTarget = audioTarget;
        super.rootNode.flipMode = QFlipV;
        _cbQueue = cbQueue;
    }
    return self;
}

- (void)dealloc
{
    NSLog(@"%@ dealloc ", self);
}

- (id<QVideoTarget>)playerView {
    return [super.mediaFactory videoTarget];
}

- (void)setPlayerView:(id<QVideoTarget>)playerView{
    [playerView setVideoRender:self];
    super.videoTarget = playerView;
}

- (QEditorPlayerState)state
{
    return coreStateToXMState(_pPlayer->getPlayerState());
}

- (int64_t)position
{
    return _pPlayer->getPosition();
}

- (void)setLoopPlay:(BOOL)loopPlay
{
//    _pPlayer->getScheduler()->setIsLoop(loopPlay?true:false);
}

- (BOOL)loopPlay
{
//    return _pPlayer->getScheduler()->isLoop()?YES:NO;
    return false;
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

#pragma mark Player Control
- (BOOL)start
{
    _pPlayer->prepare();
    return TRUE;
}

- (BOOL)play
{
    _pPlayer->start();
    return YES;
}

- (BOOL)pause
{
    _pPlayer->pause();
    return YES;
}

- (BOOL)stop
{
    _pPlayer->stop();
    return TRUE;
}

- (BOOL)seekTo:(CGFloat)timePoint :(int)flags
{
    _pPlayer->seek(timePoint, flags);
    return YES;
}

- (BOOL)isPaused
{
    return _pPlayer->getUserPaused();
}

- (bool)onVideoCreate {
    return ((VideoRender*)_pPlayer.get())->onRenderCreate();
}

- (void)onVideoDestroy {
    ((VideoRender*)_pPlayer.get())->onRenderDestroy();
}

- (bool)onVideoRender:(int64_t)wantTime {
    return ((VideoRender*)_pPlayer.get())->onVideoRender(wantTime);
}

- (void)setDisplayMode:(QDisplayMode)mode viewW:(int)viewW viewH:(int)viewH {
    return ((VideoRender*)_pPlayer.get())->setDisplayMode((DisplayMode)mode, viewW, viewH);
}

- (bool)onAudioRender:(uint8_t *const)buffer needBytes:(unsigned int)needBytes wantTime:(int64_t)wantTime {
    return ((AudioRender*)_pPlayer.get())->onAudioRender(buffer, needBytes, wantTime);
}

@end
