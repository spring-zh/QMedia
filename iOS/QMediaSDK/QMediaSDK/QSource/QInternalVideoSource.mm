//
//  QInternalVideoSource.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QInternalVideoSource.h"
#import <AVFoundation/AVFoundation.h>

#include <memory>
#include "Utils/rw_lock.h"
#include "QVideoFrame.h"
#include "QAudioFrame.h"
#include "EffectEditor/MediaSource.h"

extern QVideoDescribe* videoDescribeToXM(struct VideoDescribe desc);
extern QAudioDescribe* audioDescribeToXM(struct AudioDescribe desc);

static NSString * FourCCString(FourCharCode code) {
    NSString *result = [NSString stringWithFormat:@"%c%c%c%c",
                        (code >> 24) & 0xff,
                        (code >> 16) & 0xff,
                        (code >> 8) & 0xff,
                        code & 0xff];
    NSCharacterSet *characterSet = [NSCharacterSet whitespaceCharacterSet];
    return [result stringByTrimmingCharactersInSet:characterSet];
}

static MediaDescribe AVAssetTrackToMediaDescribe(AVAssetTrack* avTrack){
    MediaDescribe mediadescribe;
    if (NSOrderedSame == [avTrack.mediaType compare:AVMediaTypeVideo]) {
        CGSize dimensions = CGSizeApplyAffineTransform(avTrack.naturalSize,avTrack.preferredTransform);
        CMFormatDescriptionRef desc = (__bridge CMFormatDescriptionRef)avTrack.formatDescriptions.firstObject;
        FourCharCode subType = CMFormatDescriptionGetMediaSubType(desc);
        if (subType == kCMVideoCodecType_H264) {
            mediadescribe._videodesc.codectype = VideoCodecType::kH264;
        }else if (subType == kCMVideoCodecType_HEVC) {
            mediadescribe._videodesc.codectype = VideoCodecType::kH265;
        }
        mediadescribe.mediatype_ = MediaType::Video;
        mediadescribe._videodesc.width = dimensions.width;
        mediadescribe._videodesc.height = dimensions.height;
        mediadescribe._videodesc.framerate = avTrack.nominalFrameRate;
        mediadescribe._videodesc.bitrate = avTrack.estimatedDataRate;
        mediadescribe._videodesc.timeScale = kNumMillisecsPerSec;
    }else if (NSOrderedSame == [avTrack.mediaType compare:AVMediaTypeAudio]) {
        CMAudioFormatDescriptionRef item = (__bridge CMAudioFormatDescriptionRef)avTrack.formatDescriptions.firstObject;
        const  AudioStreamBasicDescription *audioStreamDescription = CMAudioFormatDescriptionGetStreamBasicDescription(item);
        NSString* subType = FourCCString(CMFormatDescriptionGetMediaSubType(item));
        if ([subType containsString:@"aac"]) {
            mediadescribe._audiodesc.codectype = AudioCodecType::kAAC;
        }
        mediadescribe.mediatype_ = MediaType::Audio;
        mediadescribe._audiodesc.samplerate = audioStreamDescription->mSampleRate;
        mediadescribe._audiodesc.nchannel = audioStreamDescription->mChannelsPerFrame;
        mediadescribe._audiodesc.bitwidth = audioStreamDescription->mBitsPerChannel;
        mediadescribe._audiodesc.bitrate = avTrack.estimatedDataRate;
    }
    return mediadescribe;
}


//
class AssetReaderSource : public MediaSource {
public:
    explicit AssetReaderSource(std::string filePath):_file_path(filePath),_isInit(false){}
    ~AssetReaderSource() {
        _inputAsset = nil;
        _assetReader = nil;
        _videoTrack = nil;
        _audioTrack = nil;
        _videoOutput = nil;
        _audioOutput = nil;
    }
    
    virtual bool load() override {
//        wrlock_guard wrlock(_rwlock);
        if (_isInit) return true;
        
        NSString *path = [NSString stringWithUTF8String:_file_path.c_str()];
        NSURL *url = [NSURL fileURLWithPath:path];
        if ([[path lowercaseString] hasPrefix:@"file:"]) {
            url = [NSURL URLWithString:path];
        }
        
        NSDictionary *inputOptions = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
        _inputAsset = [[AVURLAsset alloc] initWithURL:url options:inputOptions];

        CMTime time = [_inputAsset duration];
        _media_duration = 1000*time.value/time.timescale;
            
        _mediaDescs.clear();
        
        _videoTrack = [[_inputAsset tracksWithMediaType:AVMediaTypeVideo] firstObject];
        if (_videoTrack != nil) {
//            AVAssetTrack* track = videoTracks[0];
            _mediaDescs.push_back(AVAssetTrackToMediaDescribe(_videoTrack));
        }
        _audioTrack = [[_inputAsset tracksWithMediaType:AVMediaTypeAudio] firstObject];
        if (_audioTrack != nil) {
            _mediaDescs.push_back(AVAssetTrackToMediaDescribe(_audioTrack));
        }

        _isInit = true;
        return true;
    }
    virtual void unload() override {
//        wrlock_guard wrlock(_rwlock);
        if (! _isInit) return;
        
        _media_duration = 0;
        _videoTrack = nil;
        _audioTrack = nil;
        _inputAsset = nil;
        
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
//        wrlock_guard wrlock(_rwlock);
        if (! _isInit) return false;
        MediaSource::start(startMSec, endMSec);
        cancelPrepare();
        if(prepareWithTimeRange(_media_time_range))
            _isStarted = true;
        return _isStarted;
    }
    virtual void stop() override {
//        wrlock_guard wrlock(_rwlock);
        if (! _isInit) return;
        cancelPrepare();
        _isStarted = false;
    }
    virtual bool seekTo(int64_t mSec) override {
//        wrlock_guard wrlock(_rwlock);
        if (! _isInit) return false;
        if (mSec >= _media_time_range._start && mSec < _media_time_range._end) {
            Range<int64_t> prepareTimeRange = _media_time_range;
            prepareTimeRange._start = mSec;
            cancelPrepare();
            return prepareWithTimeRange(prepareTimeRange);
        }
        return false;
    }
    virtual bool isEOF() override {
//        wrlock_guard wrlock(_rwlock);
        bool fileEof = true;
        if (_assetReader && _assetReader.status != AVAssetReaderStatusCompleted) {
            fileEof = false;
        }
        return fileEof;
    }

    virtual VideoFrameDrawer* createVideoFrameDrawer() override {
        return new PixelFrameNV12Drawer(getVideoTarget());
    }
    
    /**
     * @param error .succeed 0 , -1 system error , 1 stream end
     * this function is blocking while get frame or get error code
     */
    virtual VideoFrame readNextVideoFrame(int& error) override {
//        rdlock_guard wrlock(_rwlock);
        error = -1;
        VideoFrame videoFrame(nullptr,0);
        CMSampleBufferRef sampleBufferRef = NULL;
        if (_isStarted && _assetReader.status == AVAssetReaderStatusReading)
        {
            sampleBufferRef = [_videoOutput copyNextSampleBuffer];
        
            if (sampleBufferRef) {
                error = 0;
                videoFrame = makeVideoFrame(sampleBufferRef);
                CMSampleBufferInvalidate(sampleBufferRef);
                CFRelease(sampleBufferRef);
            }
        }else if (_assetReader.status == AVAssetReaderStatusCompleted){
            error = 1;
        }
        
        return videoFrame;
    }
    virtual AudioFrame readNextAudioFrame(int& error) override {
//        rdlock_guard wrlock(_rwlock);
        error = -1;
        AudioFrame audioFrame(nullptr,0,0);
        CMSampleBufferRef sampleBufferRef = NULL;
        if (_isStarted && _assetReader.status == AVAssetReaderStatusReading)
        {
            sampleBufferRef = [_audioOutput copyNextSampleBuffer];
        
            if (sampleBufferRef) {
                error = 0;
                audioFrame = makeAudioFrame(sampleBufferRef);
                CMSampleBufferInvalidate(sampleBufferRef);
                CFRelease(sampleBufferRef);
            }
 
        }else if (_assetReader.status == AVAssetReaderStatusCompleted){
            error = 1;
        }
        
        return audioFrame;
    }
    
protected:
    bool prepareWithTimeRange(Range<int64_t> time_range){
        
        NSError *error = nil;
        _assetReader = [[AVAssetReader alloc] initWithAsset:_inputAsset error:&error];
        //set output formats
        if (_videoTrack != nil)
        {
            NSMutableDictionary *outputSettings = [NSMutableDictionary dictionary];
            [outputSettings setObject:@(kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange) forKey:(id)kCVPixelBufferPixelFormatTypeKey];
            [outputSettings setObject:[NSNumber numberWithInt:640] forKey:(id)kCVPixelBufferWidthKey];
            [outputSettings setObject:[NSNumber numberWithInt:480] forKey:(id)kCVPixelBufferHeightKey];
#if 0
            if (_videoTarget) {
               
            }
#endif
            // Maybe set alwaysCopiesSampleData to NO on iOS 5.0 for faster video decoding
            AVAssetReaderTrackOutput *readerVideoTrackOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:_videoTrack outputSettings:outputSettings];
            readerVideoTrackOutput.alwaysCopiesSampleData = NO;
            [_assetReader addOutput:readerVideoTrackOutput];
        }
        
        if (_audioTrack != nil)
        {
            NSMutableDictionary *outputSettings = nil;
            if(_audioTarget){
                // This might need to be extended to handle movies with more than one audio track
                outputSettings = [NSMutableDictionary dictionary];
                // PCM S16
                [outputSettings setObject:@(kAudioFormatLinearPCM) forKey:AVFormatIDKey];
                [outputSettings setObject:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsNonInterleaved];
                [outputSettings setObject:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsBigEndianKey];
                [outputSettings setObject:[NSNumber numberWithBool:NO] forKey:AVLinearPCMIsFloatKey];
                [outputSettings setObject:[NSNumber numberWithInt:16] forKey:AVLinearPCMBitDepthKey];
                
                // sampleRate channels
                [outputSettings setObject:[NSNumber numberWithInt:_audioTarget->getSampleRate()] forKey:AVSampleRateKey];
                [outputSettings setObject:[NSNumber numberWithInt:_audioTarget->getChannels()] forKey:AVNumberOfChannelsKey];
            }
            
            AVAssetReaderTrackOutput *readerAudioTrackOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:_audioTrack outputSettings:outputSettings];
            
            readerAudioTrackOutput.alwaysCopiesSampleData = NO;
            [_assetReader addOutput:readerAudioTrackOutput];
            
        }
        
        bool bRet = true;
        // set time range
        CMTimeRange timeRange = CMTimeRangeMake(CMTimeMake(time_range._start, 1000), CMTimeMake(time_range.getLength(), 1000));
        _assetReader.timeRange = timeRange;
        
        for( AVAssetReaderOutput *output in _assetReader.outputs ) {
            if( [output.mediaType isEqualToString:AVMediaTypeAudio] ) {
                _audioOutput = output;
            }
            else if( [output.mediaType isEqualToString:AVMediaTypeVideo] ) {
                _videoOutput = output;
            }
        }
        
        if ([_assetReader startReading] == NO)
        {
            NSLog(@"Error reading from file at URL: %s", _file_path.c_str());
            NSLog(@"\n->Error: %@", _assetReader.error);
            bRet = false;
        }
        
        if (_assetReader.status != AVAssetReaderStatusReading) {
            NSLog(@"Error reader status");
            bRet = false;
        }
        
        return bRet;
    }
    void cancelPrepare()
    {
        _videoOutput = nil;
        _audioOutput = nil;
        if (_assetReader) {
            [_assetReader cancelReading];
            _assetReader = nil;
        }
    }
    
    inline VideoFrame makeVideoFrame(CMSampleBufferRef sampleBuffer) {
        PixelFrameBuffer* pixelFB = new PixelFrameBuffer(sampleBuffer);
        VideoFrame videoFrame(std::shared_ptr<VideoFrameBuffer>(pixelFB), pixelFB->_time_stamp, pixelFB->_time_stamp);
        return videoFrame;
    }
    
    inline AudioFrame makeAudioFrame(CMSampleBufferRef sampleBuffer) {
        BlockFrameBuffer * blockFB = new BlockFrameBuffer(sampleBuffer);
        AudioFrame audioFrame(std::shared_ptr<AudioFrameBuffer>(blockFB), blockFB->_time_stamp, blockFB->_time_stamp);
        return audioFrame;
    }
    
    bool _isInit;
    
//    rwlock _rwlock;
    
    std::string _file_path;

    AVURLAsset *_inputAsset;
    AVAssetReader *_assetReader;
    AVAssetTrack *_videoTrack;
    AVAssetTrack *_audioTrack;
    AVAssetReaderOutput *_videoOutput;
    AVAssetReaderOutput *_audioOutput;
};


@implementation QInternalVideoSource {
    MediaSourceRef _source;
    QAudioDescribe* _adesc;
    QVideoDescribe* _vdesc;
}

- (instancetype)initWithFilePath:(NSString *)filePath
{
    self = [super init];
    if (self) {
        _source = MediaSourceRef(new AssetReaderSource([filePath UTF8String]));
    }
    return self;
}

- (void)dealloc
{
}

-(QVideoDescribe*)videoDesc
{
    if (_source->isInit() && !_vdesc) {
        for(auto& desc : _source->getMediaDescribe())
        {
            if(desc.mediatype_ == MediaType::Video)
            {
                _vdesc = videoDescribeToXM(desc._videodesc);
                break;
            }
        }
    }
    return _vdesc;
    
}

-(QAudioDescribe*)audioDesc
{
    if (_source->isInit() && !_adesc) {
        for(auto& desc : _source->getMediaDescribe())
        {
            if(desc.mediatype_ == MediaType::Audio)
            {
                _adesc = audioDescribeToXM(desc._audiodesc);
                break;
            }
        }
    }
    return _adesc;
}

- (MediaSourceRef)native
{
    return _source;
}

@end
