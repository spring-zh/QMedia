## **QEditorPlayer** 预览播放器， 定义如下
用于视频编辑预览展示与播放控制  
是组合器的派生实现, 参考 **[QCombiner](QCombiner_ios.md)** 实现
```objc
@interface QEditorPlayer : QCombiner <QVideoRender ,QAudioRender>

//@cbQueue 回调使用的消息队列， 如果直接用init初始化则使用mainqueue
- (instancetype)initWithQueue:(dispatch_queue_t)cbQueue;

//播放器当前状态
@property (nonatomic, readonly) QEditorPlayerState state;

//当前播放位置， 单位ms
@property (nonatomic, readonly) int64_t position;
@property (nonatomic, readonly) BOOL isPaused;
@property (nonatomic) BOOL loopPlay;

//设置播放预览view
@property (nonatomic, strong) id<QVideoTarget> playerView;

- (void)addObserver:(id<QEditorPlayerObserver>)observer;
- (void)removeObserver:(id<QEditorPlayerObserver>)observer;
- (void)removeAllObservers;

- (BOOL)start; //启动预览(默认启动后是暂停状态)
- (BOOL)play; //开始播放
- (BOOL)pause; //暂停
- (BOOL)stop; //停止
/**
 *  @timePoint : 单位ms
 *  @flags : 暂时不用
 */
- (BOOL)seekTo:(CGFloat)timePoint :(int)flags; 

@end

/**
 * QEditorPlayerObserver 预览播放器回调
 */
@protocol QEditorPlayerObserver <NSObject>
@optional
//player start后进入
- (void)onPrepare; 

//播放器状态改变
- (void)onPlayerChangedFromOldState:(NSNumber*)xmsOldState
                         toNewState:(NSNumber*)xmsNewState;

//进度回调
- (void)onPlayerTimeProgressUpdated:(NSNumber*)cgfTime;

- (void)onPlayerSeekedTo:(NSNumber*)cgfTime;

//播放完成回调
- (void)onCompleted;
@end
```