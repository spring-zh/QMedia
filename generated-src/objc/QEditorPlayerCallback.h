// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_session.djinni

#import <Foundation/Foundation.h>


@protocol QEditorPlayerCallback

- (void)onPrepare:(int32_t)code;

- (void)onStarted:(int32_t)code;

- (void)onStoped:(int32_t)code;

- (void)onSeekTo:(int64_t)timeMs;

- (void)onProgressUpdated:(int64_t)timeMs;

- (void)onCompleted:(int32_t)code;

- (void)onEvent:(int32_t)eventid
            msg:(nonnull NSDictionary<NSString *, NSString *> *)msg;

@end