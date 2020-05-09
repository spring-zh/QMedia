//
//  QAssetReaderSource.m
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAssetReaderSource.h"
#import <AVFoundation/AVFoundation.h>

static NSString * FourCCString(FourCharCode code) {
    NSString *result = [NSString stringWithFormat:@"%c%c%c%c",
                        (code >> 24) & 0xff,
                        (code >> 16) & 0xff,
                        (code >> 8) & 0xff,
                        code & 0xff];
    NSCharacterSet *characterSet = [NSCharacterSet whitespaceCharacterSet];
    return [result stringByTrimmingCharactersInSet:characterSet];
}

static QMediaDescribe* AVAssetTrackToMediaDescribe(AVAssetTrack* avTrack){
    QMediaDescribe* mediadescribe = [[QMediaDescribe alloc] init];
    if (NSOrderedSame == [avTrack.mediaType compare:AVMediaTypeVideo]) {
        QVideoDescribe* videoDescribe = [[QVideoDescribe alloc] init];
        CGSize dimensions = CGSizeApplyAffineTransform(avTrack.naturalSize,avTrack.preferredTransform);
        CMFormatDescriptionRef desc = (__bridge CMFormatDescriptionRef)avTrack.formatDescriptions.firstObject;
        FourCharCode subType = CMFormatDescriptionGetMediaSubType(desc);
        if (subType == kCMVideoCodecType_H264) {
            videoDescribe.codec = QVideoCodecH264;
        }else if (subType == kCMVideoCodecType_HEVC) {
            videoDescribe.codec = QVideoCodecH265;
        }
        mediadescribe.mediaType = QMediaTypeVideo;
        videoDescribe.width = dimensions.width;
        videoDescribe.height = dimensions.height;
        videoDescribe.framerate = avTrack.nominalFrameRate;
        videoDescribe.bitrate = avTrack.estimatedDataRate;
        videoDescribe.timeScale = 1000;
        mediadescribe.videoDescribe = videoDescribe;
    }else if (NSOrderedSame == [avTrack.mediaType compare:AVMediaTypeAudio]) {
        QAudioDescribe* audioDescribe = [[QAudioDescribe alloc] init];
        CMAudioFormatDescriptionRef item = (__bridge CMAudioFormatDescriptionRef)avTrack.formatDescriptions.firstObject;
        const  AudioStreamBasicDescription *audioStreamDescription = CMAudioFormatDescriptionGetStreamBasicDescription(item);
        NSString* subType = FourCCString(CMFormatDescriptionGetMediaSubType(item));
        if ([subType containsString:@"aac"]) {
            audioDescribe.codec = QAudioCodecAAC;
        }
        mediadescribe.mediaType = QMediaTypeAudio;
        audioDescribe.samplerate = audioStreamDescription->mSampleRate;
        audioDescribe.nchannel = audioStreamDescription->mChannelsPerFrame;
        int bitwidth = audioStreamDescription->mBitsPerChannel;
        QRawAudioFormat sample_format = QRawAudioFormatUnknown;
        if (audioStreamDescription->mFormatFlags & kAudioFormatFlagIsFloat) {
            sample_format = QRawAudioFormatFLOAT;
        }else {
            switch (bitwidth) {
                case 8:
                    sample_format = QRawAudioFormatU8;
                    break;
                case 16:
                    if (audioStreamDescription->mFormatFlags & kAudioFormatFlagIsSignedInteger) {
                        sample_format = QRawAudioFormatS16;
                    }else
                        sample_format = QRawAudioFormatU16;
                    break;
                case 32:
                    if (audioStreamDescription->mFormatFlags & kAudioFormatFlagIsSignedInteger) {
                        sample_format = QRawAudioFormatS32;
                    }else
                        sample_format = QRawAudioFormatU32;
                    break;
                default:
                    break;
            }

        }
        audioDescribe.rawFormat = sample_format;
        audioDescribe.bitrate = avTrack.estimatedDataRate;
        mediadescribe.audioDescribe = audioDescribe;
    }
    return mediadescribe;
}

@implementation QAssetReaderSource {
    QAudioDescribe* _adesc;
    QVideoDescribe* _vdesc;
    
    NSString * _path;
    NSURL *_url;
    AVURLAsset *_inputAsset;
    AVAssetReader *_assetReader;
    AVAssetTrack *_videoTrack;
    AVAssetTrack *_audioTrack;
    AVAssetReaderOutput *_videoOutput;
    AVAssetReaderOutput *_audioOutput;
    
    bool _isInit;
    bool _isStarted;
    int64_t _media_duration;
    NSRange _media_time_range;
}

- (instancetype)initWithFilePath:(NSString *)filePath
{
    self = [super init];
    if (self) {
        _path = [filePath copy];
        _url = [NSURL fileURLWithPath:filePath];
        if ([[filePath lowercaseString] hasPrefix:@"file:"]) {
            _url = [NSURL URLWithString:filePath];
        }
        _video_frame_format = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
        _isInit = false;
        _isStarted = false;
        _media_duration = 0;
    }
    return self;
}

- (void)dealloc
{
}

-(QVideoDescribe*)videoDesc
{
    return _vdesc;
}

-(QAudioDescribe*)audioDesc
{
    return _adesc;
}

@synthesize video_frame_format = _video_frame_format;
@synthesize videoTarget = _videoTarget;
@synthesize audioTarget = _audioTarget;

- (bool)load
{
    if (_isInit) return true;
    
    NSDictionary *inputOptions = [NSDictionary dictionaryWithObject:[NSNumber numberWithBool:YES] forKey:AVURLAssetPreferPreciseDurationAndTimingKey];
    _inputAsset = [[AVURLAsset alloc] initWithURL:_url options:inputOptions];

    CMTime time = [_inputAsset duration];
    _media_duration = 1000*time.value/time.timescale;
    
    _videoTrack = [[_inputAsset tracksWithMediaType:AVMediaTypeVideo] firstObject];
    if (_videoTrack != nil) {
        QMediaDescribe* mediaDescribe = AVAssetTrackToMediaDescribe(_videoTrack);
        _vdesc = mediaDescribe.videoDescribe;
    }
    _audioTrack = [[_inputAsset tracksWithMediaType:AVMediaTypeAudio] firstObject];
    if (_audioTrack != nil) {
        QMediaDescribe* mediaDescribe = AVAssetTrackToMediaDescribe(_audioTrack);
        _adesc = mediaDescribe.audioDescribe;
    }

    _isInit = true;
    return true;
}
- (void)unload
{
    if (! _isInit) return;
    
    _media_duration = 0;
    _videoTrack = nil;
    _audioTrack = nil;
    _inputAsset = nil;
    
    _isInit = false;
}
- (bool)isInit
{
    return _isInit;
}
- (bool)start:(int64_t)startMSec end:(int64_t)endMSec
{
    if (! _isInit) return false;
    
    _media_time_range.location = startMSec;
    _media_time_range.length = endMSec - startMSec;
    
    [self cancelPrepare];
    if([self prepareWithTimeRange:_media_time_range])
        _isStarted = true;
    return _isStarted;
}
- (void)stop
{
    if (!_isInit) return;
    [self cancelPrepare];
    _isStarted = false;
}
- (bool)seekTo:(int64_t)mSec
{
    if (! _isInit) return false;
    
    int64_t rangeEnd = _media_time_range.location + _media_time_range.length;
    if (mSec >= _media_time_range.location && mSec < rangeEnd) {
        NSRange newRange;
        newRange.location = mSec;
        newRange.length = rangeEnd;
        [self cancelPrepare];
        return [self prepareWithTimeRange:newRange];
    }
    return false;
}
- (bool)isEOF
{
    bool fileEof = true;
    if (_assetReader && _assetReader.status != AVAssetReaderStatusCompleted) {
        fileEof = false;
    }
    return fileEof;
}
- (bool)isStarted
{
    return _isStarted;
}
- (int64_t)getMediaDuration
{
    return _media_duration;
}

- (SampleBufferFrame)readNextVideoFrame
{
    SampleBufferFrame sampleFrame;
    sampleFrame.error = -1;
    sampleFrame.invalid_pts = false;
    CMSampleBufferRef sampleBufferRef = NULL;
    if (_isStarted && _assetReader.status == AVAssetReaderStatusReading)
    {
        sampleBufferRef = [_videoOutput copyNextSampleBuffer];
    
        if (sampleBufferRef) {
            sampleFrame.error = 0;
        }
    }else if (_assetReader.status == AVAssetReaderStatusCompleted){
        sampleFrame.error = 1;
    }
    sampleFrame.sampleBuffer = sampleBufferRef;
    return sampleFrame;
}
- (SampleBufferFrame)readNextAudioFrame
{
    SampleBufferFrame sampleFrame;
    sampleFrame.error = -1;
    sampleFrame.invalid_pts = false;
    CMSampleBufferRef sampleBufferRef = NULL;
    if (_isStarted && _assetReader.status == AVAssetReaderStatusReading)
    {
        sampleBufferRef = [_audioOutput copyNextSampleBuffer];
    
        if (sampleBufferRef) {
            sampleFrame.error = 0;
        }
    }else if (_assetReader.status == AVAssetReaderStatusCompleted){
        sampleFrame.error = 1;
    }
    sampleFrame.sampleBuffer = sampleBufferRef;
    return sampleFrame;
}

- (bool)prepareWithTimeRange:(NSRange)time_range{
        
    NSError *error = nil;
    _assetReader = [[AVAssetReader alloc] initWithAsset:_inputAsset error:&error];
    //set output formats
    if (_videoTrack != nil)
    {
        NSMutableDictionary *outputSettings = [NSMutableDictionary dictionary];
        [outputSettings setObject:@(_video_frame_format) forKey:(id)kCVPixelBufferPixelFormatTypeKey];
//        [outputSettings setObject:[NSNumber numberWithInt:640] forKey:(id)kCVPixelBufferWidthKey];
//        [outputSettings setObject:[NSNumber numberWithInt:480] forKey:(id)kCVPixelBufferHeightKey];
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
            [outputSettings setObject:[NSNumber numberWithInt:[_audioTarget getSampleRate]] forKey:AVSampleRateKey];
            [outputSettings setObject:[NSNumber numberWithInt:[_audioTarget getChannels]] forKey:AVNumberOfChannelsKey];
        }
        
        AVAssetReaderTrackOutput *readerAudioTrackOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:_audioTrack outputSettings:outputSettings];
        
        readerAudioTrackOutput.alwaysCopiesSampleData = NO;
        [_assetReader addOutput:readerAudioTrackOutput];
        
    }
    
    bool bRet = true;
    // set time range
    CMTimeRange timeRange = CMTimeRangeMake(CMTimeMake(time_range.location, 1000), CMTimeMake(time_range.length, 1000));
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
        NSLog(@"Error reading from file at URL: %@", _url);
        NSLog(@"\n->Error: %@", _assetReader.error);
        bRet = false;
    }
    
    if (_assetReader.status != AVAssetReaderStatusReading) {
        NSLog(@"Error reader status");
        bRet = false;
    }
    
    return bRet;
}
- (void)cancelPrepare
{
    _videoOutput = nil;
    _audioOutput = nil;
    if (_assetReader) {
        [_assetReader cancelReading];
        _assetReader = nil;
    }
}


- (NSDictionary*)serialize {
    NSDictionary* dic = [[NSMutableDictionary alloc] init];
    [dic setValue:NSStringFromClass(self.class) forKey:@"objectType"];
    [dic setValue:_path forKey:@"path"];
    return dic;
}

@end
