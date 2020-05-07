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
#import "QCombiner_internal.h"
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
    std::unique_ptr<EditorExporter> _exporter;
    std::unique_ptr<ExporterCallBackImpl> _exporterCallback;
    dispatch_queue_t _cbQueue;
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
    _exporter = std::unique_ptr<EditorExporter>(new EditorExporter());
    if ((self = [super initWithNative:_exporter.get()]) != nil) {
        _observerHoster = [[QObserverHoster alloc] init];
        
        _exporterCallback = std::unique_ptr<ExporterCallBackImpl>(new ExporterCallBackImpl(self));
        _exporter->setCallBack(_exporterCallback.get());
        
        super.videoTarget = videoTarget;
        super.audioTarget = audioTarget;
        [videoTarget setVideoRender:self];
        [audioTarget setAudioRender:self];
        
        _cbQueue = cbQueue;
    }
    return self;
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
