//
//  VideoCaptureSession.m
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "VideoCaptureSession.h"

@interface VideoCaptureSession()<AVCaptureVideoDataOutputSampleBufferDelegate, AVCaptureAudioDataOutputSampleBufferDelegate>

@end

@implementation VideoCaptureSession {
    AVCaptureSession *_captureSession;
    AVCaptureDevice *_inputCamera;
    AVCaptureDevice * _inputMicphone;
    AVCaptureConnection *_connection;
    AVCaptureDeviceInput *_videoInput;
    AVCaptureDeviceInput *_audioInput;
    
    AVCaptureVideoDataOutput *_videoDataOutput;
    AVCaptureAudioDataOutput *_audioDataOutput;
    
    //video config
    int _videoFormat;
    AVCaptureSessionPreset _cameraPreset;
    AVCaptureDevicePosition _cameraPosition;
    
    //audio config
    AVAudioFormat *_audioFormat;
}

@synthesize delegate = _delegate;

- (instancetype)init
{
    if ((self = [super init]) != nil) {

        _videoFormat = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
        _cameraPreset = AVCaptureSessionPreset640x480;
        _cameraPosition = AVCaptureDevicePositionBack;
        
        _audioFormat = nil;
    }
    return self;
}

- (int)videoFormat{
    return _videoFormat;
}
- (AVCaptureSessionPreset)cameraPreset{
    return _cameraPreset;
}
- (AVCaptureDevicePosition)cameraPosition{
    return _cameraPosition;
}
- (AVAudioFormat*)audioFormat{
    return _audioFormat;
}

- (void)setVideoConfig:(AVCaptureSessionPreset)preset position:(AVCaptureDevicePosition)position format:(int)videoformat
{
    _cameraPreset = preset;
    _cameraPosition = position;
    _videoFormat = videoformat;
}
- (void)setAudioConfig:(AVAudioFormat*)audioFormat
{
    _audioFormat = audioFormat;
}

- (BOOL)initVideoCapture {
    dispatch_queue_t videoCaptureQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
    _captureSession = [[AVCaptureSession alloc]init];
    if([_captureSession canSetSessionPreset:_cameraPreset]){
        [_captureSession setSessionPreset:_cameraPreset];
    }
    
    [_captureSession beginConfiguration];
    _inputCamera = [self cameraWithPosition:_cameraPosition];
    
    NSError *error = nil;
    //initialize an AVCaptureDeviceInput with camera (AVCaptureDevice)
    _videoInput = [[AVCaptureDeviceInput alloc] initWithDevice:_inputCamera error:&error];
    if(error){
        NSAssert(NO, @"Camera error");
        return NO;
    }
    
    //add video input to AVCaptureSession
    if([_captureSession canAddInput:_videoInput]){
        [_captureSession addInput:_videoInput];
    }
    else{
        return NO;
    }
    
    //initialize an AVCaptureVideoDataOuput instance
    _videoDataOutput = [[AVCaptureVideoDataOutput alloc]init];
    [_videoDataOutput setAlwaysDiscardsLateVideoFrames:NO];
    [_videoDataOutput setVideoSettings:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange] forKey:(id)kCVPixelBufferPixelFormatTypeKey]];
    [_videoDataOutput setSampleBufferDelegate:self queue:videoCaptureQueue];
    
    //add video data output to capture session
    if([_captureSession canAddOutput:_videoDataOutput]){
        [_captureSession addOutput:_videoDataOutput];
    }
    else{
        return NO;
    }
    
    //setting orientaion
    _connection = [_videoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    _connection.videoOrientation = AVCaptureVideoOrientationPortrait;
    //notice 视频稳定参数默认为off,启用该功能会导致延时（后置摄像头720P/1080P很明显）
    //    if ([_connection isVideoStabilizationSupported]) {
    //        _connection.preferredVideoStabilizationMode = AVCaptureVideoStabilizationModeAuto;
    //    }
    _connection.videoScaleAndCropFactor = _connection.videoMaxScaleAndCropFactor;
    return YES;
}

- (BOOL)initAudioCapture {
    dispatch_queue_t audioCaptureQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0);
    NSError *error = nil;
    //get an AVCaptureDevice for audio, here we want micphone
    _inputMicphone = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
    
    //intialize the AVCaputreDeviceInput instance with micphone device
    _audioInput =[[AVCaptureDeviceInput alloc]initWithDevice:_inputMicphone error:&error];
    if(error){
        NSLog(@"micphone error");
        return NO;
    }
    
    //add audio device input to capture session
    if([_captureSession canAddInput:_audioInput]){
        [_captureSession addInput:_audioInput];
    }else{
        return NO;
    }
    
    //initliaze an AVCaptureAudioDataOutput instance and set to
    _audioDataOutput = [[AVCaptureAudioDataOutput alloc] init];
    
    if (_audioFormat) {
        
        //        AVAudioFormat *audioFormat = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:44100 channels:1];
        NSDictionary *audioFormatDict = [NSDictionary dictionaryWithObjectsAndKeys:_audioFormat,AVFormatIDKey, nil];
        [_audioDataOutput setValuesForKeysWithDictionary:audioFormatDict];
    }
    
    if([_captureSession canAddOutput:_audioDataOutput]){
        [_captureSession addOutput:_audioDataOutput];
    }else{
        return NO;
    }
    
    [_audioDataOutput setSampleBufferDelegate:self queue:audioCaptureQueue];
    return YES;
}

- (bool)startCapture
{
    BOOL bRet = YES;
    if (_enableVideo) {
        bRet &= [self initVideoCapture];
    }
    
    if (_enableAudio) {
        bRet &= [self initAudioCapture];
    }
    
    if (bRet) {
        [_captureSession commitConfiguration];
        [_captureSession startRunning];
    }
    return bRet;
}
- (void)stopCapture
{
    [_captureSession stopRunning];
}

- (AVCaptureDevice *)cameraWithPosition:(AVCaptureDevicePosition)position
{
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *device in devices )
        if ( device.position == position )
            return device;
    return nil;
}

-(void)updateVideoOrientation{
//    if(_camera_position == AVCaptureDevicePositionFront)
//        _connection.videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
//    else if(_camera_position == AVCaptureDevicePositionBack)
//        _connection.videoOrientation = AVCaptureVideoOrientationLandscapeRight;
    
    //setting orientaion
    _connection = [_videoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    _connection.videoOrientation = AVCaptureVideoOrientationPortrait;
    if ([_connection isVideoStabilizationSupported]) {
        _connection.preferredVideoStabilizationMode = AVCaptureVideoStabilizationModeAuto;
    }
    _connection.videoScaleAndCropFactor = _connection.videoMaxScaleAndCropFactor;
}

- (bool)swapFrontAndBackCameras {
    // Assume the session is already running
    
    Boolean retVal = YES;
    NSArray *inputs = _captureSession.inputs;
    for (AVCaptureDeviceInput *input in inputs ) {
        AVCaptureDevice *device = input.device;
        if ( [device hasMediaType:AVMediaTypeVideo] ) {
            AVCaptureDevicePosition position = device.position;
            AVCaptureDevice *newCamera =nil;
            AVCaptureDeviceInput *newInput =nil;
            
            if (position ==AVCaptureDevicePositionFront)
                _cameraPosition = AVCaptureDevicePositionBack;
            else
                _cameraPosition = AVCaptureDevicePositionFront;
            
            newCamera = [self cameraWithPosition:_cameraPosition];
            NSError *error = nil;
            newInput = [AVCaptureDeviceInput deviceInputWithDevice:newCamera error:&error];
            
            // beginConfiguration ensures that pending changes are not applied immediately
            [_captureSession beginConfiguration];
        
            [_captureSession removeInput:input];
            if([_captureSession canAddInput:newInput])
            {
                [_captureSession addInput:newInput];
            }else
            {
                retVal = NO;
            }
            
            [self updateVideoOrientation];
            // Changes take effect once the outermost commitConfiguration is invoked.
            [_captureSession commitConfiguration];
            break;
        }
    }
    
    return retVal;
}

-(void) captureOutput:(AVCaptureOutput *)output didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection{
    AVCaptureVideoOrientation orientation = connection.videoOrientation;
    if(output == _videoDataOutput){
        if(_delegate && [self.delegate respondsToSelector:@selector(processVideoSampleBuffer:)]){
            [_delegate processVideoSampleBuffer:sampleBuffer];
        }
    }else if(output == _audioDataOutput){
        if(_delegate &&[self.delegate respondsToSelector:@selector(processAudioSampleBuffer:)]){
            [_delegate processAudioSampleBuffer:sampleBuffer];
        }
    }
}

@end
