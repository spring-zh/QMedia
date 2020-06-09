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

- (void)dealloc {
    NSLog(@"%@ dealloc ", self);
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

- (QMediaTrack*)createVideoTrack:(NSString *)filePath combiner:(QCombiner*)combiner
{
    QAssetReaderSource* readerSource = [[QAssetReaderSource alloc] initWithFilePath:filePath];
    readerSource.videoTarget = _videoTarget;
    readerSource.audioTarget = _audioTarget;
//    readerSource.video_frame_format = kCVPixelFormatType_32BGRA;
    QMediaTrack* mediaTrack = [[QMediaTrack alloc] initWithMediaSource:readerSource];
    if (mediaTrack) {
        [mediaTrack generateVideoTrackNode:combiner];
        [mediaTrack generateAudioTrackNode:combiner];
    }
    return mediaTrack;
}

- (QMediaTrack*)createAudioTrack:(NSString *)filePath combiner:(QCombiner*)combiner
{
    QAssetReaderSource* readerSource = [[QAssetReaderSource alloc] initWithFilePath:filePath audio:true video:false];
    readerSource.videoTarget = _videoTarget;
    readerSource.audioTarget = _audioTarget;
//    readerSource.video_frame_format = kCVPixelFormatType_32BGRA;
    QMediaTrack* mediaTrack = [[QMediaTrack alloc] initWithMediaSource:readerSource];
    if (mediaTrack) {
        [mediaTrack generateAudioTrackNode:combiner];
    }
    return mediaTrack;
}

- (QMediaTrack*)createCaptureTrack:(AVCaptureSessionPreset)preset position:(AVCaptureDevicePosition)position video:(bool)enableVideo audio:(bool)enableAudio combiner:(QCombiner*)combiner;
{
    QCaptureSource* captureSource = [[QCaptureSource alloc] initWithPreset:preset position:position];
    captureSource.videoTarget = _videoTarget;
    captureSource.audioTarget = _audioTarget;
    captureSource.enableVideo = enableVideo;
    captureSource.enableAudio = enableAudio;
    QMediaTrack* mediaTrack = [[QMediaTrack alloc] initWithMediaSource:captureSource];
    if (mediaTrack) {
        if(enableVideo)[mediaTrack generateVideoTrackNode:combiner];
        if(enableAudio)[mediaTrack generateAudioTrackNode:combiner];
    }
    return mediaTrack;
}

- (QMediaTrack*)createOldVideoTrack:(NSString *)filePath combiner:(QCombiner*)combiner{
    QInternalVideoSource *xmSource = [[QInternalVideoSource alloc] initWithFilePath:filePath];
    [xmSource native]->setVideoTarget(_videoTargetAdapter.get());
    [xmSource native]->setAudioTarget(_audioTargetAdapter.get());

    QMediaTrack* mediaTrack = [[QMediaTrack alloc] initWithMediaSourceNative:[xmSource native]] ;
    if (mediaTrack) {
        [mediaTrack generateVideoTrackNode:combiner];
        [mediaTrack generateAudioTrackNode:combiner];
    }
    return mediaTrack;
}

@end
