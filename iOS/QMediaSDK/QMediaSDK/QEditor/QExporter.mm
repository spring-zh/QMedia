//
//  QExporter.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QExporter.h"
#import "QCommon.h"
#import "QObserverHoster.h"
#import "QAudioTarget_internal.h"
#import "QVideoTarget_internal.h"
#import "QGraphicNode_internal.h"
#import "QMediaTrack_internal.h"
#import "QMediaFactory_internal.h"
#include "EffectEditor/EditorExporter.h"

extern const struct VideoDescribe XMToVideoDescribe(QVideoDescribe* xmdesc);
extern const struct AudioDescribe XMToAudioDescribe(QAudioDescribe* xmdesc);

@interface QExporter()
@property (nonatomic) QObserverHoster* observerHoster;
@property (nonatomic, readonly) dispatch_queue_t cbQueue;
@end

#pragma mark - Core Exporter Callback
class ExporterCallBackImpl: public EditorExporter::Callback {
public:
    ExporterCallBackImpl(QExporter *exporter):_xmExporter(exporter) {}
    ~ExporterCallBackImpl() {}
    
    void onStarted(RetCode code) override {
        dispatch_async(_xmExporter.cbQueue, ^(){
            @autoreleasepool{
                [_xmExporter.observerHoster notifyObservers:@selector(onExporterStarted:)
                                            withObject:@((QExporterRetCode)code)];
            }
        });
    }
    void onStoped(RetCode code) override {
        dispatch_async(_xmExporter.cbQueue, ^(){
            @autoreleasepool{
                [_xmExporter.observerHoster notifyObservers:@selector(onExporterStoped:)
                                            withObject:@((QExporterRetCode)code)];
            }
        });
    }
    void onProgressUpdated(int64_t mSec) override {
        dispatch_async(_xmExporter.cbQueue, ^(){
            @autoreleasepool{
            [_xmExporter.observerHoster notifyObservers:@selector(onExporterProgressUpdated:)
                                           withObject:@([QCommonUtils millisecondsToCGFloatSeconds:mSec])];
            }
        });
    }
    void onCanceled(RetCode code) override {
        dispatch_async(_xmExporter.cbQueue, ^(){
            @autoreleasepool{
                [_xmExporter.observerHoster notifyObservers:@selector(onExporterCanceled:)
                                            withObject:@((QExporterRetCode)code)];
            }
        });
    }
    void onCompleted() override {
        dispatch_async(_xmExporter.cbQueue, ^(){
            @autoreleasepool{
                [_xmExporter.observerHoster notifyObservers:@selector(onExporterCompleted)];
            }
        });
    }
    
private:
    __weak QExporter* _xmExporter;
};

@implementation QExporter {
    NSMutableArray<QMediaTrack*>* _subObjectArray;
    std::unique_ptr<EditorExporter> _exporter;
    std::unique_ptr<ExporterCallBackImpl> _exporterCallback;
    dispatch_queue_t _cbQueue;
    id<QVideoTarget> _videoTarget;
    id<QAudioTarget> _audioTarget;
    QMediaFactory *_mediaFactory;
    QGraphicNode* _rootNode;
}

@synthesize observerHoster = _observerHoster;
@synthesize cbQueue = _cbQueue;

- (instancetype)initWithTarget:(id<QVideoTarget>)videoTarget audioTarget:(id<QAudioTarget>)audioTarget
{
    _cbQueue = dispatch_get_main_queue();
    return [self initWithTarget:videoTarget audioTarget:audioTarget cbQueue:_cbQueue];
}

- (instancetype)initWithTarget:(id<QVideoTarget>)videoTarget audioTarget:(id<QAudioTarget>)audioTarget cbQueue:(dispatch_queue_t)cbQueue
{
    if ((self = [super init]) != nil) {
        _observerHoster = [[QObserverHoster alloc] init];
        _exporter = std::unique_ptr<EditorExporter>(new EditorExporter());
        _exporterCallback = std::unique_ptr<ExporterCallBackImpl>(new ExporterCallBackImpl(self));
        _exporter->setCallBack(_exporterCallback.get());
        
        _mediaFactory = [[QMediaFactory alloc] initWithTarget:videoTarget audio:audioTarget];
        
        //set video target
        _videoTarget = videoTarget;
        _exporter->setVideoTarget(_mediaFactory.nativeVideoTarget);
        [_videoTarget setVideoRender:self];
        //set audio target
        _audioTarget = audioTarget;
        _exporter->setAudioTarget(_mediaFactory.nativeAudioTarget);
        [_audioTarget setAudioRender:self];
        
        _subObjectArray = [NSMutableArray new];
        _rootNode = [[QGraphicNode alloc] initWithNode:_exporter->getRootNode()];
        _cbQueue = cbQueue;
    }
    return self;
}

- (void)setVideoConfig:(QVideoDescribe*)describe
{
    _exporter->setvideoConfig(XMToVideoDescribe(describe));
}
- (void)setAudioConfig:(QAudioDescribe*)describe
{
    _exporter->setAudioConfig(XMToAudioDescribe(describe));
}

- (NSArray<QMediaTrack*>*)subObjects
{
    return _subObjectArray;
}

#pragma mark - Observer Logic
- (void)addObserver:(id<QExporterObserver>)observer
{
    [_observerHoster addObserver:observer];
}

- (void)removeObserver:(id<QExporterObserver>)observer
{
    [_observerHoster removeObserver:observer];
}

- (void)removeAllObservers
{
    [_observerHoster removeAllObservers];
}

#pragma mark - Player Control
- (void)addMediaTrack:(QMediaTrack*)track
{
    _exporter->addMediaTrack(track.native);
    [_subObjectArray addObject:track];
}
- (void)removeMediaTrack:(QMediaTrack*)track
{
    _exporter->removeMediaTrack(track.native);
    [_subObjectArray removeObject:track];
}

- (void)start
{
    _exporter->start();
}
- (void)stop
{
    _exporter->stop();
}
- (void)cancel
{
    _exporter->cancel();
}

- (int64_t)position
{
    return _exporter->getPosition();
}

- (NSRange)mediaTimeRange
{
    NSRange nsRange;
    Range<int64_t> tRange = _exporter->getMediaTimeRange();
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

- (EditorExporter*)native{
    return _exporter.get();
}

- (bool)onVideoCreate {
    return ((VideoRender*)_exporter.get())->onRenderCreate();
}

- (void)onVideoDestroy {
    ((VideoRender*)_exporter.get())->onRenderDestroy();
}

- (bool)onVideoRender:(int64_t)wantTime {
    return ((VideoRender*)_exporter.get())->onVideoRender(wantTime);
}

- (bool)onAudioRender:(uint8_t *const)buffer needBytes:(unsigned int)needBytes wantTime:(int64_t)wantTime {
    return ((AudioRender*)_exporter.get())->onAudioRender(buffer, needBytes, wantTime);
}

@end
