//
//  QVideoTrackNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QVideoTrackNode.h"
#import "QGraphicNode_internal.h"
#import "QMediaTrack_internal.h"

@implementation QVideoTrackNode {
    MediaGraphicChannelRef _mediaGraphicNode;
    __weak QMediaTrack *_mediaTrack;
}

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner{
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initFromTrack:mediaTrack combiner:combiner uid:(__bridge_transfer NSString *)uuid];
}

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner uid:(NSString*)uid {
    if (! mediaTrack.native->getVideoDescribe()) //media source doesn't contain video channel
        return nil;
    _mediaTrack = mediaTrack;
    _mediaGraphicNode = MediaGraphicChannelRef(new MediaGraphicChannel(mediaTrack.native.get()));
    return (self = [super initWithNode:_mediaGraphicNode combiner:combiner uid:uid]);
}

- (QMediaTrack*)mediaTrack {
    return _mediaTrack;
}

- (void)dealloc {
    NSLog(@"%@ dealloc", self);
}

@end
