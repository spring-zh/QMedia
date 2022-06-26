//
//  QFileExporterTarget.m
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QFileRunLoop.h"
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import "QGLContext.h"
#import "IOSTexture.h"

static dispatch_queue_attr_t getDefaultQueueAttribute(void)
{
#if TARGET_OS_IPHONE
    if ([[[UIDevice currentDevice] systemVersion] compare:@"9.0" options:NSNumericSearch] != NSOrderedAscending)
    {
        return dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_SERIAL, QOS_CLASS_DEFAULT, 0);
    }
#endif
    return nil;
}

static void runAsynchronously(dispatch_queue_t processingQueue, const void *key, void (^block)(void))
{
    if (dispatch_get_specific(key))
    {
        block();
    }else
    {
        dispatch_async(processingQueue, block);
    }
}

static void runSynchronously(dispatch_queue_t processingQueue, const void *key, void (^block)(void))
{
    if (dispatch_get_specific(key))
    {
        block();
    }else
    {
        dispatch_sync(processingQueue, block);
    }
}


@implementation QFileRunLoop {
    NSString *_filePath;
    AVAssetWriter *_writer;
    AVAssetWriterInput* _writerVideoInput;
    AVAssetWriterInput* _writerAudioInput;
    AVAssetWriterInputPixelBufferAdaptor *_writerPixelBufferInput;
    
    QVideoEncodeOption* _venc_opt;
    QAudioDescription* _audio_description;
    
    __weak QAudioRender* _audio_render;
    __weak QVideoRender* _video_render;
    
    dispatch_queue_t _videoCbQueue;
    dispatch_queue_t _audioCbQueue;
    
    dispatch_queue_t _glContextQueue;
    void* _glContextQueueKey;
    CGSize _requestResolution;
    bool _isAllKeyFrame;
    bool _encodingLiveVideo;
    
    CMFormatDescriptionRef _audioFormatDescription;
    
    QGLContext* _glContext;
//    GraphicCore::ShaderProgram _shaderProgram;
    IOSFastTexture* _encodeTexture;
    GLuint _framebuffer;
    GLuint _movieRenderbuffer;
    
    float _video_duration;
    int64_t _audio_write_bytes;
    int64_t _video_pts;
    int64_t _audio_pts;
    
    bool _isRecording;
    bool _alreadyFinishedRecording;
    bool _videoEncodingIsFinished;
    bool _audioEncodingIsFinished;
    
    NSData* _audio_in_data;
}

@synthesize hw_samplerate = _hw_samplerate;

- (instancetype)initWithPath:(NSString*)filePath {
    if (self = [super init]) {
        _filePath = [filePath copy];
        _video_pts = 0;
        _audio_pts = 0;
        
        _audio_render = nil;
        _video_render = nil;
        
        _venc_opt = nil;
        _audio_description = nil;
        
        _isAllKeyFrame = false;
        _encodingLiveVideo = false;
        _framebuffer = 0;
        _audio_in_data = nil;
        
        _isRecording = false;
        _alreadyFinishedRecording = true;
        _videoEncodingIsFinished = true;
        _audioEncodingIsFinished = true;
        _audioFormatDescription = nil;
        
        [self initializeMovie];
        
    }
    return self;
}

- (void)dealloc {
    NSLog(@"%@ dealloc ", self);
    _videoCbQueue = nil;
    _audioCbQueue = nil;
    _glContextQueue = nil;
    _glContext = nil;
}

#pragma mark AssetWriter

- (bool)initializeMovie
{
    NSLog(@"QFileRunLoop initializeMovie");
    
    bool ret = false;
    
    NSError *error = nil;
    NSURL *url = [NSURL fileURLWithPath:_filePath];
    _writer = [[AVAssetWriter alloc] initWithURL:url fileType:AVFileTypeMPEG4 error:&error];
    if (error == nil)
    {
        // Set this to make sure that a functional movie is produced, even if the recording is cut off mid-stream. Only the last second should be lost in that case.
        _writer.movieFragmentInterval = kCMTimeInvalid;//CMTimeMakeWithSeconds(1.0, 1000);
        if (!_isAllKeyFrame) {
            _writer.shouldOptimizeForNetworkUse = YES;
        }
        
        //init main queue
        _glContextQueueKey = &_glContextQueueKey;
        _glContextQueue = dispatch_queue_create("com.QMedia.MovieWriter.openGLESContextQueue", getDefaultQueueAttribute());
        
        dispatch_queue_set_specific(_glContextQueue, _glContextQueueKey, (__bridge void*)self, NULL);
        
        ret = true;
    }
    else
    {
        NSLog(@"initializeMovie Error: %s", [error.localizedDescription UTF8String]);
    }
    return ret;
}

- (bool)setVideoInput:(int)width height:(int)height {
    NSLog(@"QFileRunLoop setVideoInput width:%d height:%d", width, height);
    void(^block)(void) = ^{
        // use default output settings if none specified
        NSMutableDictionary *outputSettings = [NSMutableDictionary dictionary];
        [outputSettings setObject:AVVideoCodecH264 forKey:AVVideoCodecKey];
        [outputSettings setObject:[NSNumber numberWithInt:width] forKey:AVVideoWidthKey];
        [outputSettings setObject:[NSNumber numberWithInt:height] forKey:AVVideoHeightKey];
        
        NSMutableDictionary *compressionProperties = [NSMutableDictionary dictionary];
        [compressionProperties setObject:[NSNumber numberWithInt: _venc_opt.bitrate]  forKey:AVVideoAverageBitRateKey];
        if (_isAllKeyFrame) {
            [compressionProperties setObject:[NSNumber numberWithInt:1]  forKey:AVVideoMaxKeyFrameIntervalKey];
        }
        [outputSettings setObject:compressionProperties forKey:AVVideoCompressionPropertiesKey];
        
        _writerVideoInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo outputSettings:outputSettings];
        _writerVideoInput.expectsMediaDataInRealTime = _encodingLiveVideo;
        
        // You need to use BGRA for the video in order to get realtime encoding. I use a color-swizzling shader to line up glReadPixels' normal RGBA output with the movie input's BGRA.
        NSDictionary* pixelBufferOptions = @{
//            (NSString *) kCVPixelBufferOpenGLESCompatibilityKey : @YES,
//            (NSString *) kCVPixelBufferOpenGLESTextureCacheCompatibilityKey : @YES,
//            (NSString *) kCVPixelBufferIOSurfacePropertiesKey : @{},
            (NSString *) kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_32BGRA),
            (NSString *) kCVPixelBufferWidthKey : @(width),
            (NSString *) kCVPixelBufferHeightKey : @(height)
        };

        _writerPixelBufferInput = [[AVAssetWriterInputPixelBufferAdaptor alloc]
                                   initWithAssetWriterInput:_writerVideoInput
                                   sourcePixelBufferAttributes: pixelBufferOptions];
        
        [_writer addInput:_writerVideoInput];
    };
    
    runSynchronously(_glContextQueue, _glContextQueueKey, block);
    return true;
}

- (bool)setAudioInput:(int)channels sampleRate:(int)sampleRate {
    NSLog(@"QFileRunLoop setAudioInput channels:%d sampleRate:%d", channels, sampleRate);
    //TODO: init AudioStreamBasicDescription for encode
    AudioStreamBasicDescription audioFormat;
    // PCM S16
    audioFormat.mFormatID   = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
    audioFormat.mBitsPerChannel = 16;
    // sampleRate channels
    audioFormat.mSampleRate = sampleRate;
    audioFormat.mChannelsPerFrame = channels;
    audioFormat.mFramesPerPacket = 1;
    audioFormat.mBytesPerFrame = (audioFormat.mBitsPerChannel / 8) * audioFormat.mChannelsPerFrame;
    audioFormat.mBytesPerPacket = audioFormat.mBytesPerFrame * audioFormat.mFramesPerPacket;
    audioFormat.mReserved = 0;
    
    AudioChannelLayout acl;
    bzero(&acl, sizeof(acl));
    acl.mChannelLayoutTag = kAudioChannelLayoutTag_Stereo;//kAudioChannelLayoutTag_Mono;
    
    CMAudioFormatDescriptionCreate(kCFAllocatorDefault,
                                   &audioFormat,
                                   sizeof(acl), &acl, 0, NULL, NULL,
                                   &_audioFormatDescription
                                   );

    
    void(^block)(void) = ^{
         
        AVAudioSession *sharedAudioSession = [AVAudioSession sharedInstance];
        double preferredHardwareSampleRate;
        
        if ([sharedAudioSession respondsToSelector:@selector(sampleRate)])
        {
            preferredHardwareSampleRate = [sharedAudioSession sampleRate];
        }
        else
        {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
            preferredHardwareSampleRate = [[AVAudioSession sharedInstance] currentHardwareSampleRate];
#pragma clang diagnostic pop
        }
        _hw_samplerate = preferredHardwareSampleRate;
        _audio_description.sampleRate = _hw_samplerate;
        
        AudioChannelLayout acl;
        bzero( &acl, sizeof(acl));
        acl.mChannelLayoutTag = kAudioChannelLayoutTag_Stereo;
        
        NSDictionary *audioOutputSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                             [ NSNumber numberWithInt: kAudioFormatMPEG4AAC], AVFormatIDKey,
                                             [ NSNumber numberWithInt: channels ], AVNumberOfChannelsKey,
                                             [ NSNumber numberWithFloat: preferredHardwareSampleRate ], AVSampleRateKey,
                                             [ NSData dataWithBytes: &acl length: sizeof( acl ) ], AVChannelLayoutKey,
                                             //[ NSNumber numberWithInt:AVAudioQualityLow], AVEncoderAudioQualityKey,
                                             [ NSNumber numberWithInt: 64000 ], AVEncoderBitRateKey,
                                             nil];
        
        _writerAudioInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeAudio outputSettings:audioOutputSettings];
        _writerAudioInput.expectsMediaDataInRealTime = _encodingLiveVideo;
        [_writer addInput:_writerAudioInput];
        
        const int bufferlen = 1024 * _audio_description.nchannel * 2;
        _audio_in_data = [[NSMutableData alloc] initWithLength:bufferlen];
    };
    
    runSynchronously(_glContextQueue, _glContextQueueKey, block);
    return true;
}

- (bool)initGL {
    NSLog(@"QFileRunLoop initGL");
    
    __block bool ret = true;

    runSynchronously(_glContextQueue, _glContextQueueKey, ^{
        _glContext = [[QGLContext alloc] init];
        [_glContext useSharegroup:[[[QGLContext sharedGLContext] context] sharegroup]];
        
        [_glContext useAsCurrentContext];
 
        glGenFramebuffers(1, &_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
        if(! [QGLContext supportsFastTextureUpload]) {
            glGenRenderbuffers(1, &_movieRenderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, _movieRenderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, _venc_opt.width, _venc_opt.height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _movieRenderbuffer);
        }
        
        [_video_render OnViewSizeChange:_venc_opt.width height:_venc_opt.height];
        
        [EAGLContext setCurrentContext:nil];
    });
    
    return ret;
}

- (void)releaseGL {
    if (!_glContext) return;
    NSLog(@"QFileRunLoop releaseGL");
     runSynchronously(_glContextQueue, _glContextQueueKey, ^{
         [_glContext useAsCurrentContext];
         [_video_render onViewDestroy];

         glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,0,0);
         if(_framebuffer) glDeleteFramebuffers(1, &_framebuffer);
         glBindFramebuffer(GL_FRAMEBUFFER, 0);
//             [_encodeTexture releaseTexture];
         _encodeTexture = nil;
         if(_movieRenderbuffer)glDeleteRenderbuffers(1, &_movieRenderbuffer);
         _glContext = nil;
         [EAGLContext setCurrentContext:nil];

     });
}

- (bool)startVideoRecording {
    NSLog(@"QFileRunLoop startVideoRecording");
    __block BOOL bRet = TRUE;
    void(^videoInputReadyCallback)(void) = ^{
        _videoEncodingIsFinished = false;
        _videoCbQueue = dispatch_queue_create("com.QMedia.MovieWriter.videoReadingQueue", getDefaultQueueAttribute());
        [_writerVideoInput requestMediaDataWhenReadyOnQueue:_videoCbQueue usingBlock:^{
            NSLog(@"video requestMediaDataWhenReadyOnQueue begin");
            while (!_videoEncodingIsFinished && _writer.status ==  AVAssetWriterStatusWriting ){
                if (_writerVideoInput.readyForMoreMediaData) {
                    if(![self handleVideoRequest]){
                        break;
                    }
                }
            }
            NSLog(@"requestMediaDataWhenReadyOnQueue exit, _writer.status %ld", (long)_writer.status);
        }];
    };
    
    runSynchronously(_glContextQueue, _glContextQueueKey, ^{
        bRet = [self startRecording:0];
        videoInputReadyCallback();
    });
    return bRet;
}

- (bool)startAudioRecording {
    NSLog(@"QFileRunLoop startAudioRecording");
    __block BOOL bRet = TRUE;
    void(^audioInputReadyCallback)(void) = ^{
        _audioEncodingIsFinished = false;
        _audioCbQueue = dispatch_queue_create("com.QMedia.MovieWriter.audioReadingQueue", getDefaultQueueAttribute());
           [_writerAudioInput requestMediaDataWhenReadyOnQueue:_audioCbQueue usingBlock:^{
               NSLog(@"audio requestMediaDataWhenReadyOnQueue begin");
               while (!_audioEncodingIsFinished && _writer.status ==  AVAssetWriterStatusWriting && _writerAudioInput.readyForMoreMediaData){
                   if(![self handleAudioRequest]){
                       break;
                   }
               }
           }];
    };
    
    runSynchronously(_glContextQueue, _glContextQueueKey, ^{
        bRet = [self startRecording:0];
        audioInputReadyCallback();
    });
    return bRet;
}

- (BOOL)startRecording:(int64_t)startTime
{
    BOOL bRet = FALSE;
    _isRecording = true;
    
    NSLog(@"QFileRunLoop startRecording writer status:%ld", (long)_writer.status);
    if( _writer.status != AVAssetWriterStatusWriting &&
       _writer.status != AVAssetWriterStatusFailed)
    {
        bRet = [_writer startWriting];
        
//        const int32_t preferredTimescale = 1000000;
//        CMTime frameTime = CMTimeMakeWithSeconds(0, preferredTimescale);
        CMTime frameTime = CMTimeMake(startTime, 1000);
        [_writer startSessionAtSourceTime:frameTime];
    }
    return bRet;
}

- (void)stopVideoRecording
{
    NSLog(@"QFileRunLoop stopVideoRecording ");
    _videoEncodingIsFinished = true;
    if (!_writerVideoInput.readyForMoreMediaData) {
        return;
    }
    dispatch_sync(_videoCbQueue, ^(){
        if (_writer.status == AVAssetWriterStatusCompleted)
        {
            return;
        }
            
        [_writerVideoInput markAsFinished];
     });
    
    [self cancelRecording:true];
}

- (void)stopAudioRecording
{
    NSLog(@"QFileRunLoop stopAudioRecording ");
    _audioEncodingIsFinished = true;
    if (!_writerAudioInput.readyForMoreMediaData) {
        return;
    }
    dispatch_sync(_audioCbQueue, ^(){
        if (_writer.status == AVAssetWriterStatusCompleted)
        {
            return;
        }
            
        [_writerAudioInput markAsFinished];
     });
    [self cancelRecording:true];
}

- (void)cancelRecording:(bool)finish{
    NSLog(@"QFileRunLoop cancelRecording");
    __block bool isWaiting = false;
    NSCondition *condition = [[NSCondition alloc]init];
    void(^finishedBlock)(void) = ^{
        [condition lock];
            isWaiting = false;
            [condition signal];
        [condition unlock];
    };
    
    if (_audioEncodingIsFinished && _videoEncodingIsFinished) {
        runSynchronously(_glContextQueue, _glContextQueueKey, ^{
            _alreadyFinishedRecording = true;
            if (finish && _writer.status == AVAssetWriterStatusWriting) {
                isWaiting = true;
                [_writer finishWritingWithCompletionHandler:finishedBlock];
            }else
                [_writer cancelWriting];
        });
    }
    
    [condition lock];
        while (isWaiting) {
            [condition wait];
        }
    [condition unlock];
}

- (BOOL)handleVideoRequest
{
    __block bool bRet = false;
    __block CVPixelBufferRef renderTarget = NULL;
    runSynchronously(_glContextQueue, _glContextQueueKey, ^{
        if(!_writerVideoInput.readyForMoreMediaData)
            return;
        [_glContext useAsCurrentContext];
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
        if([QGLContext supportsFastTextureUpload]) {
            _encodeTexture = [[IOSFastTexture alloc] initWithContext:_glContext size:CGSizeMake(_venc_opt.width, _venc_opt.height) pool:[_writerPixelBufferInput pixelBufferPool]];
            
            glBindTexture([_encodeTexture glTexTarget], [_encodeTexture glTexid]);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, [_encodeTexture glTexid],0);
            
            glViewport(0, 0, _encodeTexture.width, _encodeTexture.height);
        }
        else {
            //TODO: bind renderbuffer
            glBindRenderbuffer(GL_RENDERBUFFER, _movieRenderbuffer);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _movieRenderbuffer);
            glViewport(0, 0, _venc_opt.width, _venc_opt.height);
        }
        
        const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            NSAssert(NO,@"glCheckFramebufferStatus: %d", status);
        }

        bRet = [_video_render onDraw:_video_pts];

        [[_glContext context] presentRenderbuffer:GL_RENDERBUFFER];
        if(![QGLContext supportsFastTextureUpload]) {
        
            CVPixelBufferPoolCreatePixelBuffer (kCFAllocatorDefault, [_writerPixelBufferInput pixelBufferPool], &renderTarget);
            CVPixelBufferLockBaseAddress(renderTarget, 0);
            GLubyte *pixelBufferData = (GLubyte *)CVPixelBufferGetBaseAddress(renderTarget);
            //FIXME: check if support GL_BGRA
            glReadPixels(0, 0, _venc_opt.width, _venc_opt.height, GL_BGRA, GL_UNSIGNED_BYTE, pixelBufferData);
            CVPixelBufferUnlockBaseAddress(renderTarget, 0);
        }
        [EAGLContext setCurrentContext:nil];
    });
    
    if (bRet) {
        const int32_t preferredTimescale = 1000;
        CMTime frameTime = CMTimeMake(_video_pts, preferredTimescale);
        _video_pts += _video_duration;
        
        bool bWrite = false;
        if([QGLContext supportsFastTextureUpload]) {
            bWrite = [_writerPixelBufferInput appendPixelBuffer:_encodeTexture.pixelBuffer withPresentationTime:frameTime];
            _encodeTexture = nil;
        }else if(renderTarget != NULL){
            bWrite = [_writerPixelBufferInput appendPixelBuffer:renderTarget withPresentationTime:frameTime];
            CVPixelBufferRelease(renderTarget);
        }
        if (!bWrite){
            NSLog(@"Problem appending pixel buffer at time: %@", CMTimeCopyDescription(kCFAllocatorDefault, frameTime));
            NSLog(@"zsperror _writerPixelBufferInput appendPixelBuffer");
        }
        bRet = bWrite;
    }else {
//        NSAssert(NO, @"onVideoRender error");
    }
    
    return bRet;
}

- (BOOL)handleAudioRequest
{
    bool bRet = false;
    _audio_pts = _audio_write_bytes*1000/(_audio_description.sampleRate * _audio_description.nchannel * 2);
    int bufferlen = _audio_in_data.length;
    if([_audio_render OnPlayBuffer:_audio_in_data sizeNeed:bufferlen wantTime:_audio_pts])
    {
        OSStatus status;
        CMBlockBufferRef blockBuffer = NULL;
        CMSampleBufferRef sampleBuffer = NULL;

        // Create sample buffer for adding to the audio input.
        status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault, NULL, bufferlen, kCFAllocatorDefault, NULL, 0, bufferlen, 0, &blockBuffer);
        if (status != noErr) {
            NSAssert(NO,@"CMBlockBufferCreateWithMemoryBlock error(%d)", (int)status);
        }
        
        status = CMBlockBufferReplaceDataBytes(_audio_in_data.bytes, blockBuffer, 0, bufferlen);
        if (kCMBlockBufferNoErr != status) {
            NSAssert(NO,@"CMBlockBufferReplaceDataBytes error(%d)", (int)status);
        }
        
        int numSamples = bufferlen / (_audio_description.nchannel * 2);
        const int32_t preferredTimescale = _audio_description.sampleRate;
        CMTime timestamp = CMTimeMakeWithSeconds(_audio_pts/1000, preferredTimescale);
        CMSampleTimingInfo timing = { CMTimeMake(1, _audio_description.sampleRate), timestamp, kCMTimeInvalid };
        // S16
        size_t sizeArray[1] = {2};
        status = CMSampleBufferCreate(kCFAllocatorDefault,
                                      blockBuffer,
                                      YES, nil, nil,
                                      _audioFormatDescription,
                                      numSamples,
                                      1, &timing,
                                      1, sizeArray,
                                      &sampleBuffer);
        
        _audio_write_bytes += bufferlen;
        CMTime currentSampleTime = CMSampleBufferGetOutputPresentationTimeStamp(sampleBuffer);
        bRet = [_writerAudioInput appendSampleBuffer:sampleBuffer];
        usleep(10 * 1000);
        if (!bRet) {
            NSLog(@"Problem appending audio buffer at time: %@", CFBridgingRelease(CMTimeCopyDescription(kCFAllocatorDefault, currentSampleTime)));
            NSLog(@"zsperror _writerAudioInput appendSampleBuffer");
        }

        CFRelease(blockBuffer);
        CMSampleBufferInvalidate(sampleBuffer);
        CFRelease(sampleBuffer);
    }
    return bRet;
}

#pragma mark --------------- implemention <QAudioRunloop, QVideoRunloop> --------------

- (BOOL)videoInit:(nonnull QVideoEncodeOption *)venc_opt {
    _venc_opt = venc_opt;
    [_video_render setDisplayMode:0 flipV:0];
//    [_videoRender setDisplayMode:DisplayStretch viewW:_videoDesc.width viewH:_videoDesc.height];
    if(_venc_opt.framerate < 0)
        _venc_opt.framerate = 25;
    _video_duration = (float)1000 / _venc_opt.framerate;
    return [self setVideoInput:_venc_opt.width height:_venc_opt.height];
}

- (void)setVideoRender:(nullable QVideoRender *)render {
    _video_render = render;
}

- (BOOL)start {
    [self initGL];
    return [self startVideoRecording];
}

- (void)stop {
    [self stopVideoRecording];
    [self releaseGL];
}

- (void)pause:(BOOL)isPause {

}

- (void)forceUpdate {

}

- (void)setAudioRender:(nullable QAudioRender *)render {
    _audio_render = render;
}

- (BOOL)audioInit:(nonnull QAudioDescription *)desc {
    _audio_description = desc;
    _audio_write_bytes = 0;
    return [self setAudioInput:_audio_description.nchannel sampleRate:_audio_description.sampleRate];
}

- (BOOL)audioStart {
    return [self startAudioRecording];
}

- (void)audioStop {
    [self stopAudioRecording];
}

- (void)audioPause:(BOOL)bpause {

}

- (void)audioFlush {

}

- (float)getVolume {
    return 1;
}


- (void)setVolume:(float)volume {
    
}



@end

