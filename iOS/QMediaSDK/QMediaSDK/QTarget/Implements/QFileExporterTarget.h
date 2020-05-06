//
//  QFileExporterTarget.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaDescribe.h"
#import "QAudioTarget.h"
#import "QVideoTarget.h"

@interface QFileExporterTarget : NSObject <QAudioTarget, QVideoTarget>
//@property (nonatomic, copy) XMVideoDescribe* videoDesc;
//@property (nonatomic, copy) XMAudioDescribe* audioDesc;

- (instancetype)initWithPath:(NSString*)filePath;
//- (instancetype)initWithDescribe:(XMVideoDescribe*)videoDesc audioDesc:(XMAudioDescribe*)audioDesc;
- (bool)startAudio;
- (bool)stopAudio;
- (bool)startVideo;
- (bool)stopVideo;

@property (nonatomic, readonly) bool hasAudio;
@end
