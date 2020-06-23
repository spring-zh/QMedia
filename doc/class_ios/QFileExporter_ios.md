## **QFileExporter** 导出模块 定义如下
QExporter用于视频编辑结果导出合成 
是组合器的派生实现, 参考 **[QCombiner](QCombiner_ios.md)** 实现
```objc
/**
 * QExporter 文件合成器。
 * 内置输出目标为文件
 */
@interface QFileExporter : QExporter

- (instancetype)initWithPath:(NSString*)filePath;
- (instancetype)initWithPath:(NSString*)filePath cbQueue:(dispatch_queue_t)cbQueue;

@property (nonatomic, readonly) NSString* filePath;

@end

/**
 * QExporter 导出合成器。
 * 通过设置QVideoTarget和QAudioTarget， 支持使用自定义输出目标
 */

@interface QExporter : QCombiner <QVideoRender ,QAudioRender>

- (instancetype)initWithTarget:(id<QVideoTarget>)videoTarget audioTarget:(id<QAudioTarget>)audioTarget;
- (instancetype)initWithTarget:(id<QVideoTarget>)videoTarget audioTarget:(id<QAudioTarget>)audioTarget cbQueue:(dispatch_queue_t)cbQueue;

- (void)addObserver:(id<QExporterObserver>)observer;
- (void)removeObserver:(id<QExporterObserver>)observer;
- (void)removeAllObservers;

- (void)start;
- (void)stop;
- (void)cancel;

@property (nonatomic, readonly) int64_t position;

@end

/**
 * QExporterObserver 导出合成器消息回调
 */
@protocol QExporterObserver <NSObject>
@optional
- (void)onExporterStarted:(QExporterRetCode)code; //合成启动， code 0 表示成功
- (void)onExporterStoped:(QExporterRetCode)code; //合成手动结束
- (void)onExporterProgressUpdated:(CGFloat)cgfTime; //合成进度回调
- (void)onExporterCanceled:(QExporterRetCode)code; //合成取消
- (void)onExporterCompleted; //合成完成
@end
```