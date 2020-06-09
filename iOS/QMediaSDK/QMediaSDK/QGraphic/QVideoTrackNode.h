//
//  QVideoTrackNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QGraphicNode.h"

@class QMediaTrack;
/**
 * a graphic node form media track.  tips: this node will be all by QMediaTrack
*/
@interface QVideoTrackNode : QGraphicNode

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner;
- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) QMediaTrack* mediaTrack;

@end
