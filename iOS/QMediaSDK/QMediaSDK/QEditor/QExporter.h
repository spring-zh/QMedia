//
//  QExporter.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCombiner.h"

typedef NS_ENUM(NSInteger, QExporterRetCode) {
    ExporterRetCode_unknow = -100,
    ExporterRetCode_e_state = -2,
    ExporterRetCode_e_system = -1,
    ExporterRetCode_ok = 0
};

@protocol QExporterObserver <NSObject>
@optional
- (void)onExporterStarted:(QExporterRetCode)code;
- (void)onExporterStoped:(QExporterRetCode)code;
- (void)onExporterProgressUpdated:(CGFloat)cgfTime;
- (void)onExporterCanceled:(QExporterRetCode)code;
- (void)onExporterCompleted;
@end

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
