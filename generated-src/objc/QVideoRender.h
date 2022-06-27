// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from video_runloop.djinni

#import <Foundation/Foundation.h>


@interface QVideoRender : NSObject

- (void)setDisplayMode:(int32_t)mode
                 flipV:(BOOL)flipV;

- (void)onViewSizeChange:(int32_t)width
                  height:(int32_t)height;

- (void)onViewDestroy;

- (BOOL)onDraw:(int64_t)pirv
     noDisplay:(BOOL)noDisplay;

- (void)readRGBA:(nonnull id)buffer
           width:(int32_t)width
          height:(int32_t)height
          format:(int32_t)format;

@end
