//
//  QExporter.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QVideoEncodeOption.h"
#import "QAudioEncodeOption.h"
#import "QMediaRange.h"
#import "QMediaSegment.h"
#import "QCommon.h"

typedef NS_ENUM(NSInteger, QExporterRetCode) {
    Exporter_unknow = -100,
    Exporter_e_state = -2,
    Exporter_e_system = -1,
    Exporter_ok = 0
};

@class QExporter;

@protocol QExporterObserver <NSObject>
@optional
- (void)onExporterProgressUpdated:(nonnull QExporter*)exporter time:(NSNumber*)cgfTime;
- (void)onExporterStarted:(nonnull QExporter*)exporter error:(int)errcode;
- (void)onExporterStoped:(nonnull QExporter*)exporter;
- (void)onExporterCanceled:(nonnull QExporter*)exporter;
- (void)onExporterCompleted:(nonnull QExporter*)exporter error:(int)errcode;
- (void)onExporterEvent:(nonnull QExporter*)exporter eventid:(int32_t)eventid msg:(nonnull NSDictionary<NSString *, NSString *> *)msg;
@end

/** export the media edite status to a output target .
 *  only support QFileExporterTarget to set QVideoTarget and QAudioTarget  now.
 */
@interface QExporter : NSObject
- (instancetype _Nullable )initWithPath:(NSString*_Nonnull)filePath queue:(dispatch_queue_t _Nullable )cbQueue;
- (instancetype _Nullable )initWithPath:(NSString*_Nonnull)filePath;

- (void)addObserver:(id<QExporterObserver>_Nonnull)observer;
- (void)removeObserver:(id<QExporterObserver>_Nonnull)observer;
- (void)removeAllObservers;

- (nullable QMediaSegment *)cresteMediaSegment:(nonnull NSString *)filename
                                          flag:(int32_t)flag;

- (BOOL)addMediaSegment:(nullable QMediaSegment *)segment;

- (BOOL)removeMediaSegment:(nullable QMediaSegment *)segment;

- (nonnull QMediaRange *)getTotalTimeRange;

- (void)setVideoOption:(nonnull QVideoEncodeOption *)venc_opt;
- (void)setAudioOption:(nonnull QAudioEncodeOption *)aenc_opt;

- (void)start;
- (void)stop;
- (void)cancel;

@property (nonatomic, readonly) int64_t position;

@end

