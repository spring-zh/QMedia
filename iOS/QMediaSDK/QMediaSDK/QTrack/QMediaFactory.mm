//
//  QMediaFactory.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaFactory.h"
#import "QAssetReaderSource.h"
#import "QCaptureSource.h"
#import "QInternalVideoSource.h"
#import "QMediaSource_internal.h"
#import "QMediaTrack_internal.h"

@implementation QMediaFactory {
    id<QVideoTarget> _videoTarget;
    id<QAudioTarget> _audioTarget;
    std::shared_ptr<VideoTarget> _videoTargetAdapter;
    std::shared_ptr<AudioTarget> _audioTargetAdapter;
}

- (id<QVideoTarget>)videoTarget {
    return _videoTarget;
}

- (void)setVideoTarget:(id<QVideoTarget>)videoTarget {
    _videoTarget = videoTarget;
    _videoTargetAdapter = std::shared_ptr<VideoTargetAdapter>(new VideoTargetAdapter(_videoTarget));
}

- (id<QAudioTarget>)audioTarget {
    return _audioTarget;
}

- (void)setAudioTarget:(id<QAudioTarget>)audioTarget {
    _audioTarget = audioTarget;
    _audioTargetAdapter = std::shared_ptr<AudioTargetAdapter>(new AudioTargetAdapter(_audioTarget));
}

- (VideoTarget*)nativeVideoTarget
{
    return _videoTargetAdapter.get();
}

- (AudioTarget*)nativeAudioTarget
{
    return _audioTargetAdapter.get();
}

//+ (instancetype)sharedInstance {
//    static id instance = nil;
//    static dispatch_once_t onceToken;
//    dispatch_once(&onceToken, ^{
//        instance = [[super allocWithZone:NULL] init];
//    });
//    return instance;
//}

//- (instancetype)initWithTarget:(id<QVideoTarget>)videoTarget audio:(id<QAudioTarget>)audioTarget
//{
//    if ((self = [super init]) != nil) {
//        _videoTarget = videoTarget;
//        _audioTarget = audioTarget;
//        _videoTargetAdapter = std::shared_ptr<VideoTargetAdapter>(new VideoTargetAdapter(_videoTarget));
//        _audioTargetAdapter = std::shared_ptr<AudioTargetAdapter>(new AudioTargetAdapter(_audioTarget));
//    }
//    return self;
//}

- (QMediaTrack*)createVideoTrack:(NSString *)filePath
{
    QAssetReaderSource* readerSource = [[QAssetReaderSource alloc] initWithFilePath:filePath];
    readerSource.videoTarget = _videoTarget;
    readerSource.audioTarget = _audioTarget;
//    readerSource.video_frame_format = kCVPixelFormatType_32BGRA;
    return [[QMediaTrack alloc] initWithMediaSource:readerSource];
}

- (QMediaTrack*)createCaptureTrack:(AVCaptureSessionPreset)preset position:(AVCaptureDevicePosition)position;
{
    QCaptureSource* captureSource = [[QCaptureSource alloc] initWithPreset:preset position:position];
    captureSource.videoTarget = _videoTarget;
    captureSource.audioTarget = _audioTarget;
    captureSource.enableVideo = true;
    return [[QMediaTrack alloc] initWithMediaSource:captureSource];
}

- (QMediaTrack*)createOldVideoTrack:(NSString *)filePath{
    QInternalVideoSource *xmSource = [[QInternalVideoSource alloc] initWithFilePath:filePath];
    [xmSource native]->setVideoTarget(_videoTargetAdapter.get());
    [xmSource native]->setAudioTarget(_audioTargetAdapter.get());

    return [[QMediaTrack alloc] initWithMediaSourceNative:[xmSource native]] ;
}

@end
