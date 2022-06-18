//
//  QAudioPlayer.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "QAudioRunloop.h"
#import "QAudioRender.h"

@interface AudioInputBuffer : NSObject
@property (nonatomic, readonly) uint8_t* audio_data;
@property (nonatomic, readonly) int audio_size;
@end

@interface QAudioPlayer : NSObject <QAudioRunloop>

@property (nonatomic) int audioDelay;
- (QAudioRender*)getRender;

@end
