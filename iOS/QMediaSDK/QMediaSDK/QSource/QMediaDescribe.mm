//
//  QMediaDescribe.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaDescribe.h"
#include "MediaCore/core/MediaComm.h"

QVideoDescribe* videoDescribeToXM(struct VideoDescribe desc)
{
    return [[QVideoDescribe alloc] initWithParamenters:(QVideoCodec)desc.codectype framerate:desc.framerate width:desc.width height:desc.height bitrate:desc.bitrate];
}

const struct VideoDescribe XMToVideoDescribe(QVideoDescribe* xmdesc)
{
    struct VideoDescribe videoDesc;
    videoDesc.codectype = (VideoCodecType)xmdesc.codec;
    videoDesc.rotation = (VideoRotation)xmdesc.rotation;
    videoDesc.framerate = xmdesc.framerate;
    videoDesc.width = xmdesc.width;
    videoDesc.height = xmdesc.height;
    videoDesc.bitrate = xmdesc.bitrate;
    videoDesc.timeScale = xmdesc.timeScale;
    return videoDesc;
}

QAudioDescribe* audioDescribeToXM(struct AudioDescribe desc)
{
    return [[QAudioDescribe alloc] initWithParamenters:(QAudioCodec)desc.codectype rawFormat:(QRawAudioFormat)desc.format
    samplerate:desc.samplerate nchannel:desc.nchannel bitrate:desc.bitrate];
}

const struct AudioDescribe XMToAudioDescribe(QAudioDescribe* xmdesc)
{
    struct AudioDescribe audioDesc;
    audioDesc.codectype = (AudioCodecType)xmdesc.codec;
    audioDesc.format = (RawAudioFormat)xmdesc.rawFormat;
    audioDesc.samplerate = xmdesc.samplerate;
    audioDesc.nchannel = xmdesc.nchannel;
    audioDesc.bitwidth = 16;
    audioDesc.bitrate = xmdesc.bitrate;
    return audioDesc;
}

@implementation QVideoDescribe

- (instancetype)initWithParamenters:(QVideoCodec)codec framerate:(float)framerate
width:(int)width height:(int)height bitrate:(int)bitrate
{
    if (self = [super init]) {
        _codec = codec;
        _framerate = framerate;
        _width = width;
        _height = height;
        _bitrate = bitrate;
    }
    return self;
}

- (id)copyWithZone:(NSZone *)zone {
    return self;
}

@end


@implementation QAudioDescribe

- (instancetype)initWithParamenters:(QAudioCodec)codec rawFormat:(QRawAudioFormat)rawFormat
samplerate:(int)samplerate nchannel:(int)nchannel bitrate:(int)bitrate
{
    if (self = [super init]) {
        _codec = codec;
        _rawFormat = rawFormat;
        _samplerate = samplerate;
        _nchannel = nchannel;
        _bitrate = bitrate;
    }
    return self;
}

- (id)copyWithZone:(NSZone *)zone {
    return self;
}

@end

@implementation QMediaDescribe

@synthesize audioDescribe = _audioDescribe;
@synthesize videoDescribe = _videoDescribe;

@end
