//
//  QAudioNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>

@class QMediaTrack;

/**
 *  tips: this node will be all by QMediaTrack
 */
@interface QAudioTrackNode : NSObject

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack;
@property (nonatomic, readonly) QMediaTrack* mediaTrack;

@property (nonatomic) bool enable;
@property (nonatomic) float volume;

@end
