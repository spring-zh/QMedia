//
//  QCaptureSource.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCaptureSource.h"
#import "VideoCaptureSession.h"
#import <AVFoundation/AVFoundation.h>

@interface QCaptureSource() <VideoCaptureSessionDelegate>
@end

static CGSize presetToSize(AVCaptureSessionPreset preset){
    if ([preset isEqualToString:AVCaptureSessionPreset352x288]) {
        return CGSizeMake(352, 288);
    }else if ([preset isEqualToString:AVCaptureSessionPreset640x480]) {
        return CGSizeMake(640, 480);
    }else if ([preset isEqualToString:AVCaptureSessionPresetiFrame960x540]) {
        return CGSizeMake(960, 540);
    }else if ([preset isEqualToString:AVCaptureSessionPreset1280x720]) {
        return CGSizeMake(1280, 720);
    }else if ([preset isEqualToString:AVCaptureSessionPreset1920x1080]) {
        return CGSizeMake(1920, 1080);
    }
    return CGSizeMake(0, 0);
}

#if 0
extern XMVideoDescribe* videoDescribeToXM(struct VideoDescribe desc);
extern XMAudioDescribe* audioDescribeToXM(struct AudioDescribe desc);

class CaptureSource : public MediaSource {
public:
    explicit CaptureSource(XMCaptureSource* captureSoruce):
    _isInit(false),
    _xmSource(captureSoruce){
        
    }
    
    ~CaptureSource() {
    }
    
    virtual bool load() override {
        if (_isInit) return true;
        
        if (_xmSource.enableVideo) {
            CGSize resolution = presetToSize(_xmSource.captureSession.cameraPreset);
            MediaDescribe videodescribe;
            videodescribe.mediatype_ = MediaType::Video;
            videodescribe._videodesc.width = resolution.width;
            videodescribe._videodesc.height = resolution.height;
            videodescribe._videodesc.framerate = 30;
            videodescribe._videodesc.bitrate = 0;
            videodescribe._videodesc.timeScale = kNumMillisecsPerSec;
            _mediaDescs.push_back(videodescribe);
        }
        
        if (_xmSource.enableAudio) {
            MediaDescribe audiodescribe;
            audiodescribe.mediatype_ = MediaType::Audio;
            audiodescribe._audiodesc.samplerate = 44100;
            audiodescribe._audiodesc.nchannel = 1;
            audiodescribe._audiodesc.bitwidth = 16;
            audiodescribe._audiodesc.bitrate = 0;
            _mediaDescs.push_back(audiodescribe);
        }
        
        _media_duration = std::numeric_limits<int64_t>::max();
        _isInit = true;
        return true;
    }
    virtual void unload() override {
        if (! _isInit) return;
        
        _media_duration = 0;
        _mediaDescs.clear();
        
        _isInit = false;
    }
    
    virtual bool isInit() override { return _isInit; }
    
    /**
     *
     * @param startMSec base start time
     * @param endMSec base end time
     * @return bool
     */
    virtual bool start(int64_t startMSec, int64_t endMSec) override {
        if (! _isInit) return false;
        MediaSource::start(startMSec, endMSec);
        [_xmSource.captureSession startCapture];
        _isStarted = true;
        return _isStarted;
    }
    virtual void stop() override {
        if (! _isInit) return;
        [_xmSource.captureSession stopCapture];
        _isStarted = false;
    }
    virtual bool seekTo(int64_t mSec) override {
        return false;
    }
    virtual bool isEOF() override {
        return false;
    }

    virtual VideoFrameDrawer* createVideoFrameDrawer() override {
        return new PixelFrameNV12Drawer(getVideoTarget());
    }
    
    /**
     * @param error .succeed 0 , -1 system error , 1 stream end
     * this function is blocking while get frame or get error code
     */
    virtual VideoFrame readNextVideoFrame(int& error) override {
        error = -1;
        VideoFrame videoFrame(nullptr,0);
        CMSampleBufferRef sampleBufferRef = [_xmSource readLastVideoBuffer];
        if (sampleBufferRef) {
            videoFrame = makeVideoFrame(sampleBufferRef);
            CMSampleBufferInvalidate(sampleBufferRef);
            CFRelease(sampleBufferRef);
            
            error = 0;
        }else
            error = 1;
        
        return videoFrame;
    }
    virtual AudioFrame readNextAudioFrame(int& error) override {
        error = -1;
        AudioFrame audioFrame(nullptr,0,0);
        CMSampleBufferRef sampleBufferRef = [_xmSource readLastAudioBuffer];
        if (sampleBufferRef) {
            audioFrame = makeAudioFrame(sampleBufferRef);
            CMSampleBufferInvalidate(sampleBufferRef);
            CFRelease(sampleBufferRef);
            
            error = 0;
        }else
            error = 1;
        
        return audioFrame;
    }
    
protected:
    
    inline VideoFrame makeVideoFrame(CMSampleBufferRef sampleBuffer) {
        PixelFrameBuffer* pixelFB = new PixelFrameBuffer(sampleBuffer);
        VideoFrame videoFrame(std::shared_ptr<VideoFrameBuffer>(pixelFB), -1/*pixelFB->_time_stamp*/, -1/*pixelFB->_time_stamp*/);
        return videoFrame;
    }
    
    inline AudioFrame makeAudioFrame(CMSampleBufferRef sampleBuffer) {
        BlockFrameBuffer * blockFB = new BlockFrameBuffer(sampleBuffer);
        AudioFrame audioFrame(std::shared_ptr<AudioFrameBuffer>(blockFB), -1/*blockFB->_time_stamp*/, -1/*blockFB->_time_stamp*/);
        return audioFrame;
    }
    
    bool _isInit;
    

    __strong XMCaptureSource* _xmSource;
};
#endif

@implementation QCaptureSource {
    QAudioDescribe* _adesc;
    QVideoDescribe* _vdesc;
    
    VideoCaptureSession *_videoCaptureSession;
    
    AVCaptureSessionPreset _preset;
    AVCaptureDevicePosition _position;
    
    NSMutableArray *_video_buffer_queue;
    NSMutableArray *_audio_buffer_queue;
    
    bool _isInit;
    bool _isStarted;
}

@synthesize video_frame_format = _video_frame_format;

- (instancetype)initWithPreset:(AVCaptureSessionPreset)preset position:(AVCaptureDevicePosition)position;
{
    self = [super init];
    if (self) {
        _video_frame_format = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
        _preset = preset;
        _position = position;
        _videoCaptureSession = [[VideoCaptureSession alloc] init];
        _videoCaptureSession.delegate = self;
        [_videoCaptureSession setVideoConfig:_preset position:_position format:_video_frame_format];
        
        _video_buffer_queue = [[NSMutableArray alloc] init];
        _audio_buffer_queue = [[NSMutableArray alloc] init];
        _videoCaptureSession.enableAudio = false;
        _videoCaptureSession.enableVideo = false;
        
        _isInit = false;
        _isStarted = false;
    }
    return self;
}

- (void)dealloc
{
    [self clearBuffers];
}

-(QVideoDescribe*)videoDesc
{
    return _vdesc;
}

-(QAudioDescribe*)audioDesc
{
    return _adesc;
}

- (bool)load{
    if (_isInit) return true;
    
    if (_videoCaptureSession.enableVideo) {
        CGSize resolution = presetToSize(_videoCaptureSession.cameraPreset);
        _vdesc = [[QVideoDescribe alloc] initWithParamenters:QVideoCodecUnknown framerate:30 width:resolution.width height:resolution.height bitrate:0];
    }
    
    if (_videoCaptureSession.enableAudio) {
        _adesc = [[QAudioDescribe alloc] initWithParamenters:QAudioCodecUnknown rawFormat:QRawAudioFormatS16 samplerate:44100 nchannel:1 bitrate:16];
    }
    
    _isInit = true;
    return true;
}
- (void)unload{
    _vdesc = nil;
    _adesc = nil;
}
- (bool)isInit{
    return _isInit;
}

- (bool)start:(int64_t)startMSec end:(int64_t)endMSec
{
    return (_isStarted = [_videoCaptureSession startCapture]);
}
- (void)stop
{
    [_videoCaptureSession stopCapture];
    [self clearBuffers];
}
- (bool)seekTo:(int64_t)mSec
{
    return false;
}
- (bool)isEOF{
    return false;
}
- (bool)isStarted{
    return _isStarted;
}
- (int64_t)getMediaDuration{
    return std::numeric_limits<int64_t>::max();
}

- (SampleBufferFrame)readNextAudioFrame {
    SampleBufferFrame audiobufferframe;
    audiobufferframe.error = 1;
    audiobufferframe.invalid_pts = true;
    CMSampleBufferRef readbuffer = NULL;
    @synchronized (_audio_buffer_queue) {
        if (_audio_buffer_queue.count > 0) {
            readbuffer = (__bridge_retained CMSampleBufferRef)[_audio_buffer_queue objectAtIndex:(_audio_buffer_queue.count-1)];
        }
        [_audio_buffer_queue removeAllObjects];
    };
    
    if(readbuffer){
        audiobufferframe.error = 0;
    }
    audiobufferframe.sampleBuffer = readbuffer;
    return audiobufferframe;
}


- (SampleBufferFrame)readNextVideoFrame {
    SampleBufferFrame videobufferframe;
    videobufferframe.error = 1;
    videobufferframe.invalid_pts = true;
    CMSampleBufferRef readbuffer = NULL;
    @synchronized (_video_buffer_queue) {
        if (_video_buffer_queue.count > 0) {
            readbuffer = (__bridge_retained CMSampleBufferRef)[_video_buffer_queue objectAtIndex:(_video_buffer_queue.count-1)];
        }
        [_video_buffer_queue removeAllObjects];
    };
    
    if(readbuffer){
        videobufferframe.error = 0;
    }
    videobufferframe.sampleBuffer = readbuffer;
    return videobufferframe;
}

- (bool)enableVideo{
    return _videoCaptureSession.enableVideo;
}
- (bool)enableAudio{
    return _videoCaptureSession.enableAudio;
}
- (void)setEnableVideo:(bool)enableVideo{
    _videoCaptureSession.enableVideo = enableVideo;
}
- (void)setEnableAudio:(bool)enableAudio{
    _videoCaptureSession.enableAudio = enableAudio;
}

- (bool)swapFrontAndBackCameras {
    return [_videoCaptureSession swapFrontAndBackCameras];
}

- (void)clearBuffers
{
    @synchronized (_video_buffer_queue) {
        [_video_buffer_queue removeAllObjects];
    }
    @synchronized (_audio_buffer_queue) {
        [_audio_buffer_queue removeAllObjects];
    }
}

- (void)processVideoSampleBuffer:(CMSampleBufferRef)sampleBuffer
{
    @synchronized (_video_buffer_queue) {
        if (_video_buffer_queue.count >= 15) {
            while (_video_buffer_queue.count >= 4) {
                [_video_buffer_queue removeObjectAtIndex:0];
            }
        }
        [_video_buffer_queue addObject:(__bridge id)sampleBuffer];
    }
}
- (void)processAudioSampleBuffer:(CMSampleBufferRef)sampleBuffer
{
    @synchronized (_audio_buffer_queue) {
        if (_audio_buffer_queue.count >= 15) {
            while (_audio_buffer_queue.count >= 4) {
                [_audio_buffer_queue removeObjectAtIndex:0];
            }
        }
        [_audio_buffer_queue addObject:(__bridge id)sampleBuffer];
    }
}

-(VideoCaptureSession*)captureSession{
    return _videoCaptureSession;
}

@end
