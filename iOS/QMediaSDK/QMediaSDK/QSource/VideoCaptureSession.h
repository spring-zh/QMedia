//
//  VideoCaptureSession.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

@protocol VideoCaptureSessionDelegate <NSObject>
@required
- (void)processVideoSampleBuffer:(CMSampleBufferRef)sampleBuffer;
- (void)processAudioSampleBuffer:(CMSampleBufferRef)sampleBuffer;
@end

@interface VideoCaptureSession : NSObject

@property (nonatomic, readonly) int videoFormat;
@property (nonatomic, readonly) AVCaptureSessionPreset cameraPreset;
@property (nonatomic, readonly) AVCaptureDevicePosition cameraPosition;
@property (nonatomic, readonly) AVAudioFormat *audioFormat;

@property (nonatomic, assign) bool enableVideo;
@property (nonatomic, assign) bool enableAudio;

@property (nonatomic, weak) id<VideoCaptureSessionDelegate> delegate;

//TODO: we can use these video format
//kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange , kCVPixelFormatType_420YpCbCr8BiPlanarFullRange ,kCVPixelFormatType_32BGRA
- (void)setVideoConfig:(AVCaptureSessionPreset)preset position:(AVCaptureDevicePosition)position format:(int)videoformat;
- (void)setAudioConfig:(AVAudioFormat*)audioFormat;

- (bool)startCapture;
- (void)stopCapture;
- (bool)swapFrontAndBackCameras;

@end
