// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_session.djinni

#import "QMediaRange.h"
#import "QSize.h"
#import "QVec4f.h"
#import <Foundation/Foundation.h>
@class QAudioRender;
@class QMediaSegment;
@class QVideoRender;
@protocol QAudioRunloop;
@protocol QVideoRunloop;


@interface QMediaSessionInternal : NSObject

- (nullable QMediaSegment *)cresteMediaSegment:(nonnull NSString *)filename
                                          flag:(int32_t)flag;

- (BOOL)addMediaSegment:(nullable QMediaSegment *)segment;

- (BOOL)removeMediaSegment:(nullable QMediaSegment *)segment;

- (nonnull NSArray<QMediaSegment *> *)getSegments;

- (nonnull QMediaRange *)getTotalTimeRange;

- (nullable QVideoRender *)getVideoRender;

- (nullable QAudioRender *)getAudioRender;

- (void)setAudioRunLoop:(nullable id<QAudioRunloop>)audioLoop;

- (void)setVideoRunLoop:(nullable id<QVideoRunloop>)videoLoop;

- (void)setDisplayLayerSize:(nonnull QSize *)size;

- (nonnull QSize *)getDisplayLayerSize;

- (void)setBkColor:(nonnull QVec4f *)color;

- (nonnull QVec4f *)getBkColor;

/** control */
- (void)prepare;

/** start(); */
- (void)stop;

@end
