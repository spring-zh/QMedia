//
//  QExporter.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QExporter.h"
#import "QMediaEngine.h"
#import "QObserverHoster.h"
#import "QMediaSessionInternal.h"
#import "QEditorExporterInternal.h"
#import "QEditorExporterCallback.h"
#import "QFileRunLoop.h"

@interface QExporter() <QEditorExporterCallback>
@property (nonatomic) QObserverHoster* observerHoster;
@property (nonatomic, readonly) dispatch_queue_t cbQueue;
@end

@implementation QExporter {
    NSString* _filePath;
    QEditorExporterInternal* _internal;
    QMediaSessionInternal* _session;
    QFileRunLoop * _file_runloop;
    QVideoRender * _video_render;
    QAudioRender * _audio_render;
    
    QVideoEncodeOption *_venc_opt;
    QAudioEncodeOption *_aenc_opt;
}

@synthesize observerHoster = _observerHoster;
@synthesize cbQueue = _cbQueue;

- (instancetype _Nullable )initWithPath:(NSString*_Nonnull)filePath queue:(dispatch_queue_t _Nullable )cbQueue {
    if ((self = [super init]) != nil) {
        _filePath = [filePath copy];
        _cbQueue = cbQueue;
        _observerHoster = [[QObserverHoster alloc] init];
        _internal = [QMediaEngine createEditorExporterInternal];
        _session = [_internal getSession];
        _video_render = [_session getVideoRender];
        _audio_render = [_session getAudioRender];
        _file_runloop = [[QFileRunLoop alloc] initWithPath:_filePath];
        [_file_runloop setAudioRender:_audio_render];
        [_file_runloop setVideoRender:_video_render];
        [_session setAudioRunLoop:_file_runloop];
        [_session setVideoRunLoop:_file_runloop];
        
        //FIXME: call audioplayer init in session
//        [_file_runloop audioInit:[QMediaEngine audioOut]];
    }
    return self;
}

- (instancetype _Nullable )initWithPath:(NSString*_Nonnull)filePath {
    return [self initWithPath:filePath queue:dispatch_get_main_queue()];
}

- (void)dealloc {
    NSLog(@"QExporter dealloc ");
}

#pragma mark - Observer Logic
- (void)addObserver:(id<QExporterObserver>)observer {
    [_observerHoster addObserver:observer];
}

- (void)removeObserver:(id<QExporterObserver>)observer {
    [_observerHoster removeObserver:observer];
}

- (void)removeAllObservers {
    [_observerHoster removeAllObservers];
}

- (nullable QMediaSegment *)cresteMediaSegment:(nonnull NSString *)filename
                                          flag:(int32_t)flag {
    NSLog(@"QExporter cresteMediaSegment in");
    return [_session cresteMediaSegment:filename flag:flag];
}

- (BOOL)addMediaSegment:(nullable QMediaSegment *)segment {
    NSLog(@"QExporter addMediaSegment in");
    return [_session addMediaSegment:segment];
}

- (BOOL)removeMediaSegment:(nullable QMediaSegment *)segment {
    NSLog(@"QExporter removeMediaSegment in");
    return [_session removeMediaSegment:segment];
}

- (nonnull QMediaRange *)getTotalTimeRange {
    return [_session getTotalTimeRange];
}

- (int64_t)position {
    return [_internal getPosition];
}

- (void)setVideoOption:(nonnull QVideoEncodeOption *)venc_opt {
    _venc_opt = venc_opt;
    [_file_runloop videoInit:_venc_opt];
    [_session setDisplayLayerSize:[QSize sizeWithWidth:_venc_opt.width height:_venc_opt.height]];
}
- (void)setAudioOption:(nonnull QAudioEncodeOption *)aenc_opt {
    _aenc_opt = aenc_opt;
}

- (void)start {
    NSLog(@"QExporter start ");
    [_internal setCallback:self];
    [_internal start];
}
- (void)stop {
    NSLog(@"QExporter stop ");
    [_session stop];
}
- (void)cancel {
    NSLog(@"QExporter cancel ");
    [_internal cancel];
}


- (void)onCanceled {
    dispatch_async(_cbQueue, ^(){
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onExporterCanceled:)
                                        withObjects:self, nil];
            [_internal setCallback:nil];
        }
    });
}

- (void)onCompleted:(int32_t)code {
    dispatch_async(_cbQueue, ^(){
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onExporterCompleted:error:)
                                        withObjects:self, @(code), nil];
            [_internal setCallback:nil];
        }
    });
}

- (void)onEvent:(int32_t)eventid msg:(nonnull NSDictionary<NSString *,NSString *> *)msg {
    dispatch_async(_cbQueue, ^{
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onExporterEvent:eventid:msg:) withObjects:self, @(eventid), msg, nil];
        }
    });
}

- (void)onProgressUpdated:(int64_t)timeMs {
    dispatch_async(_cbQueue, ^(){
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onExporterProgressUpdated:time:) withObjects:self, @((double)[_internal getPosition]/1000), nil];
        }
    });
}

- (void)onStarted:(int32_t)code {
    dispatch_async(_cbQueue, ^(){
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onExporterStarted:error:)
                                        withObjects:self, @(code), nil];
        }
    });
}

- (void)onStoped {
    dispatch_async(_cbQueue, ^(){
        @autoreleasepool{
            [self.observerHoster notifyObservers:@selector(onExporterStoped:)
                                        withObjects:self, nil];
            [_internal setCallback:nil];
        }
    });
}

@end

