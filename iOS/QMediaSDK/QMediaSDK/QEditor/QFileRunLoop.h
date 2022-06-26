//
//  QFileRunLoop.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QVideoDescription.h"
#import "QAudioDescription.h"
#import "QAudioRunloop.h"
#import "QVideoRunloop.h"
#import "QAudioRender.h"
#import "QVideoRender.h"
#import "QVideoEncodeOption.h"
#import "QAudioEncodeOption.h"

@interface QFileRunLoop : NSObject <QAudioRunloop, QVideoRunloop>

- (instancetype _Nonnull )initWithPath:(NSString*_Nonnull)filePath;

- (BOOL)videoInit:(nonnull QVideoEncodeOption *)venc_opt;
- (BOOL)audioInit:(nonnull QAudioDescription *)desc;

@property (nonatomic, readonly) bool hasAudio;
@property (nonatomic, readonly) double hw_samplerate;
@end
