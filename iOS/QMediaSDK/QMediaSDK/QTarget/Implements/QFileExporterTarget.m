//
//  QFileExporterTarget.m
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QFileExporterTarget.h"
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import "QGLContext.h"
#import "IOSTexture.h"

dispatch_queue_attr_t getDefaultQueueAttribute(void)
{
#if TARGET_OS_IPHONE
    if ([[[UIDevice currentDevice] systemVersion] compare:@"9.0" options:NSNumericSearch] != NSOrderedAscending)
    {
        return dispatch_queue_attr_make_with_qos_class(DISPATCH_QUEUE_SERIAL, QOS_CLASS_DEFAULT, 0);
    }
#endif
    return nil;
}

void runAsynchronously(dispatch_queue_t processingQueue, const void *key, void (^block)(void))
{
    if (dispatch_get_specific(key))
    {
        block();
    }else
    {
        dispatch_async(processingQueue, block);
    }
}

void runSynchronously(dispatch_queue_t processingQueue, const void *key, void (^block)(void))
{
    if (dispatch_get_specific(key))
    {
        block();
    }else
    {
        dispatch_sync(processingQueue, block);
    }
}


@implementation QFileExporterTarget {
    NSString *_filePath;
    AVAssetWriter *_writer;
    AVAssetWriterInput* _writerVideoInput;
    AVAssetWriterInput* _writerAudioInput;
    AVAssetWriterInputPixelBufferAdaptor *_writerPixelBufferInput;
    
    QVideoDescribe* _videoDesc;
    QAudioDescribe* _audioDesc;
    
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
}

//@synthesize videoDesc = _videoDesc;
//@synthesize audioDesc = _audioDesc;

- (instancetype)initWithPath:(NSString*)filePath
{
    if (self = [super init]) {
        _filePath = [filePath copy];
        _video_pts = 0;
        _audio_pts = 0;
        
        _videoDesc = nil;
        _audioDesc = nil;
        
        _isAllKeyFrame = false;
        _encodingLiveVideo = false;
        _framebuffer = 0;
        
        _isRecording = false;
        _alreadyFinishedRecording = true;
        _videoEncodingIsFinished = true;
        _audioEncodingIsFinished = true;
        _audioFormatDescription = nil;
        
        [self initializeMovie];
        
    }
    return self;
}

- (void)dealloc
{
    _videoCbQueue = nil;
    _audioCbQueue = nil;
    _glContextQueue = nil;
    _glContext = nil;
}

@synthesize audioRender = _audioRender;
@synthesize videoRender = _videoRender;

@synthesize audioDelay = _audioDelay;

- (int)getChannels {
    if (_audioDesc) {
        return _audioDesc.nchannel;
    }
    return 0;
}

- (QRawAudioFormat)getFormat {
    if (_audioDesc) {
        return _audioDesc.rawFormat;
    }
    return QRawAudioFormatUnknown;
}

- (int)getSampleRate {
    if (_audioDesc) {
        return _audioDesc.samplerate;
    }
    return 0;
}

- (bool)initAudio:(QAudioDescribe*)describe{
    //TODO: init by _audioDesc
    _audioDesc = describe;
    _audio_write_bytes = 0;
    return [self setAudioInput:_audioDesc.nchannel sampleRate:_audioDesc.samplerate];
}

- (bool)startAudio {
    return [self startAudioRecording];
}

- (bool)stopAudio {
    [self stopAudioRecording];
    return true;
}

- (void)unInitAudio {
    
}

- (id)getContext {
    return _glContext;
}

- (bool)initVideo:(QVideoDescribe*)describe
{
    _videoDesc = describe;
    if(_videoDesc.framerate < 0)
        _videoDesc.framerate = 25;
    _video_duration = (float)1000 / _videoDesc.framerate;
    return [self setVideoInput:_videoDesc.width height:_videoDesc.height];
}
- (int)getWidth{
    return _videoDesc.width;
}
- (int)getHeight{
    return _videoDesc.height;
}
- (float)getFrameRate{
    return _videoDesc.framerate;
}

- (bool)startVideo {
    [self initGL];
    return [self startVideoRecording];
}

- (bool)stopVideo {
    [self stopVideoRecording];
    [self releaseGL];
    return true;
}

//TODO: for exporter, these functions doesn't need to impletement
- (void)flushVideo {
}

- (void)pauseVideo:(bool)isPause {
}

- (void)flushAudio {
}

- (void)pauseAudio {
}

- (void)resumeAudio {
}

- (float)getVolume {
    return 1.0f;
}

- (void)setVolume:(float)volume {
}


#pragma mark AssetWriter


- (bool)initializeMovie
{
    NSLog(@"XMFileExporterTarget initializeMovie");
    
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
    
    void(^block)(void) = ^{
        // use default output settings if none specified
        NSMutableDictionary *outputSettings = [NSMutableDictionary dictionary];
        [outputSettings setObject:AVVideoCodecH264 forKey:AVVideoCodecKey];
        [outputSettings setObject:[NSNumber numberWithInt:width] forKey:AVVideoWidthKey];
        [outputSettings setObject:[NSNumber numberWithInt:height] forKey:AVVideoHeightKey];
        
        NSMutableDictionary *compressionProperties = [NSMutableDictionary dictionary];
        [compressionProperties setObject:[NSNumber numberWithInt: _videoDesc.bitrate]  forKey:AVVideoAverageBitRateKey];
        if (_isAllKeyFrame) {
            [compressionProperties setObject:[NSNumber numberWithInt:1]  forKey:AVVideoMaxKeyFrameIntervalKey];
        }
        [outputSettings setObject:compressionProperties forKey:AVVideoCompressionPropertiesKey];
        
        _writerVideoInput = [[AVAssetWriterInput alloc] initWithMediaType:AVMediaTypeVideo outputSettings:outputSettings];
        _writerVideoInput.expectsMediaDataInRealTime = _encodingLiveVideo;
        
        // You need to use BGRA for the video in order to get realtime encoding. I use a color-swizzling shader to line up glReadPixels' normal RGBA output with the movie input's BGRA.
        NSDictionary *sourcePixelBufferAttributesDictionary = [NSDictionary dictionaryWithObjectsAndKeys: [NSNumber numberWithInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,
                                                               [NSNumber numberWithInt:width], kCVPixelBufferWidthKey,
                                                               [NSNumber numberWithInt:height], kCVPixelBufferHeightKey,
                                                               nil];
        _writerPixelBufferInput = [[AVAssetWriterInputPixelBufferAdaptor alloc]
                                   initWithAssetWriterInput:_writerVideoInput
                                   sourcePixelBufferAttributes: sourcePixelBufferAttributesDictionary];
        
        [_writer addInput:_writerVideoInput];
    };
    
    runSynchronously(_glContextQueue, _glContextQueueKey, block);
    return true;
}

- (bool)setAudioInput:(int)channels sampleRate:(int)sampleRate {
    
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
        _audioDesc.samplerate = preferredHardwareSampleRate;
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
    };
    
    runSynchronously(_glContextQueue, _glContextQueueKey, block);
    return true;
}

- (bool)initGL {
    NSLog(@"IOSMovieWriter initGL");
    
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
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, _videoDesc.width, _videoDesc.height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _movieRenderbuffer);
        }
        
        [_videoRender onVideoCreate];
        
        [EAGLContext setCurrentContext:nil];
    });
    
    return ret;
}

- (void)releaseGL {
    if (!_glContext) return;
    NSLog(@"IOSMovieWriter releaseGL");
     runSynchronously(_glContextQueue, _glContextQueueKey, ^{
         [_glContext useAsCurrentContext];
             [_videoRender onVideoDestroy];

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
    __block BOOL bRet = TRUE;
    void(^videoInputReadyCallback)(void) = ^{
        _videoEncodingIsFinished = false;
        _videoCbQueue = dispatch_queue_create("com.QMedia.MovieWriter.videoReadingQueue", getDefaultQueueAttribute());
        [_writerVideoInput requestMediaDataWhenReadyOnQueue:_videoCbQueue usingBlock:^{
            NSLog(@"video requestMediaDataWhenReadyOnQueue begin");
            while (!_videoEncodingIsFinished && _writer.status ==  AVAssetWriterStatusWriting && _writerVideoInput.readyForMoreMediaData){
                if(![self handleVideoRequest]){
                    break;
                }
            }
        }];
    };
    
    runSynchronously(_glContextQueue, _glContextQueueKey, ^{
        bRet = [self startRecording:0];
        videoInputReadyCallback();
    });
    return bRet;
}

- (bool)startAudioRecording {
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
    
    NSLog(@"startRecording writer status:%ld", (long)_writer.status);
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
    NSLog(@"IOSMovieWriter cancelRecording");
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
            _encodeTexture = [[IOSFastTexture alloc] initWithContext:_glContext size:CGSizeMake(_videoDesc.width, _videoDesc.height) pool:[_writerPixelBufferInput pixelBufferPool]];
            
            glBindTexture([_encodeTexture glTexTarget], [_encodeTexture glTexid]);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            
            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, [_encodeTexture glTexid],0);
            
            glViewport(0, 0, _encodeTexture.width, _encodeTexture.height);
        }else {
            //TODO: bind renderbuffer
            glBindRenderbuffer(GL_RENDERBUFFER, _movieRenderbuffer);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _movieRenderbuffer);
            glViewport(0, 0, _videoDesc.width, _videoDesc.height);
        }

        const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            NSAssert(NO,@"glCheckFramebufferStatus: %d", status);
        }
        
        bRet = [_videoRender onVideoRender:_video_pts];
        glFinish();
        
        
        if(![QGLContext supportsFastTextureUpload]) {
        
            CVPixelBufferPoolCreatePixelBuffer (kCFAllocatorDefault, [_writerPixelBufferInput pixelBufferPool], &renderTarget);
            CVPixelBufferLockBaseAddress(renderTarget, 0);
            GLubyte *pixelBufferData = (GLubyte *)CVPixelBufferGetBaseAddress(renderTarget);
            glReadPixels(0, 0, _videoDesc.width, _videoDesc.height, GL_RGBA, GL_UNSIGNED_BYTE, pixelBufferData);
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
        }else if(renderTarget != NULL){
//            CVPixelBufferRef renderTarget;
//            CVPixelBufferPoolCreatePixelBuffer (kCFAllocatorDefault, [_writerPixelBufferInput pixelBufferPool], &renderTarget);
//            CVPixelBufferLockBaseAddress(renderTarget, 0);
//            GLubyte *pixelBufferData = (GLubyte *)CVPixelBufferGetBaseAddress(renderTarget);
//            glReadPixels(0, 0, _videoDesc.width, _videoDesc.height, GL_RGBA, GL_UNSIGNED_BYTE, pixelBufferData);
//            CVPixelBufferUnlockBaseAddress(renderTarget, 0);
            bWrite = [_writerPixelBufferInput appendPixelBuffer:renderTarget withPresentationTime:frameTime];
            CVPixelBufferRelease(renderTarget);
        }
        if (!bWrite){
            NSLog(@"Problem appending pixel buffer at time: %@", CMTimeCopyDescription(kCFAllocatorDefault, frameTime));
            bRet = false;
        }else
            bRet = true;
    }else {
        NSAssert(NO, @"onVideoRender error");
    }
    
    return bRet;
}

- (BOOL)handleAudioRequest
{
    bool bRet = false;
    const int bufferlen = 4096;
    uint8_t cbuffer[bufferlen];
    _audio_pts = _audio_write_bytes*1000/(_audioDesc.samplerate * _audioDesc.nchannel * 2);
    if([_audioRender onAudioRender:cbuffer needBytes:bufferlen wantTime:_audio_pts])
    {
        OSStatus status;
        CMBlockBufferRef blockBuffer = NULL;
        CMSampleBufferRef sampleBuffer = NULL;

        // Create sample buffer for adding to the audio input.
        status = CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault, NULL, bufferlen, kCFAllocatorDefault, NULL, 0, bufferlen, 0, &blockBuffer);
        if (status != noErr) {
            NSAssert(NO,@"CMBlockBufferCreateWithMemoryBlock error(%d)", (int)status);
        }
        
        status = CMBlockBufferReplaceDataBytes(cbuffer, blockBuffer, 0, bufferlen);
        if (kCMBlockBufferNoErr != status) {
            NSAssert(NO,@"CMBlockBufferReplaceDataBytes error(%d)", (int)status);
        }
        
        int numSamples = bufferlen / (_audioDesc.nchannel * 2);
        const int32_t preferredTimescale = _audioDesc.samplerate;
        CMTime timestamp = CMTimeMakeWithSeconds(_audio_pts/1000, preferredTimescale);
        CMSampleTimingInfo timing = { CMTimeMake(1, _audioDesc.samplerate), timestamp, kCMTimeInvalid };
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
        if (![_writerAudioInput appendSampleBuffer:sampleBuffer])
                NSLog(@"Problem appending audio buffer at time: %@", CFBridgingRelease(CMTimeCopyDescription(kCFAllocatorDefault, currentSampleTime)));
        else
            bRet = true;

        CFRelease(blockBuffer);
        CMSampleBufferInvalidate(sampleBuffer);
        CFRelease(sampleBuffer);
    }
    return bRet;
}


@end
