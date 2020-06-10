//
//  QAudioNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>

@class QMediaTrack;
@class QCombiner;

/**
 *  tips: this node will be all by QMediaTrack
 */
@interface QAudioTrackNode : NSObject

- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner;
- (instancetype)initFromTrack:(QMediaTrack*)mediaTrack combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) QMediaTrack* mediaTrack;

//uuid for index
@property (nonatomic, readonly) NSString* uid;

@property (nonatomic) bool enable;
@property (nonatomic) float volume;

@end
