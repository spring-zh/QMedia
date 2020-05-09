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

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack {
    if (! mediaTrack.native->getVideoDescribe()) //media source doesn't contain video channel
        return nil;
    _mediaTrack = mediaTrack;
    _mediaGraphicNode = MediaGraphicChannelRef(new MediaGraphicChannel(mediaTrack.native));
    return (self = [super initWithNode:_mediaGraphicNode]);
}

- (QMediaTrack*)mediaTrack {
    return _mediaTrack;
}

- (void)dealloc {
    NSLog(@"QVideoTrackNode dealloc");
}

@end
