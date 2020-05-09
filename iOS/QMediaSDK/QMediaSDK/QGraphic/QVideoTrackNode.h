//
//  QVideoTrackNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QGraphicNode.h"
#import "QMediaTrack.h"

/**
* a graphic node form media track
*/
@interface QVideoTrackNode : QGraphicNode

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack;
@property (nonatomic, readonly) QMediaTrack* mediaTrack;

@end
