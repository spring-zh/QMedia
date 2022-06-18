//
//  QVideoTarget.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import "QMediaDescribe.h"

typedef NS_ENUM(NSInteger, QDisplayMode2) {
    DisplayStretch = 0,
    DisplayAdaptive,
    DisplayClip
};

@protocol QVideoRender2 <NSObject>
@required
- (bool)onVideoRender:(int64_t)wantTime;
- (bool)onVideoCreate;
- (void)onVideoDestroy;

- (void)setDisplayMode:(QDisplayMode2)mode viewW:(int)viewW viewH:(int)viewH;
@end

@protocol QVideoTarget <NSObject>
@required
- (bool)startVideo;
- (bool)stopVideo;
- (void)pauseVideo:(bool)isPause;
- (void)flushVideo;

- (bool)initVideo:(QVideoDescribe*)describe;
- (int)getWidth;
- (int)getHeight;
- (float)getFrameRate;
- (id)getContext;

@property (nonatomic, weak) id<QVideoRender2> videoRender;

@end
