//
//  QMediaDescribe.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, QMediaType) {
    QMediaTypeVideo = 1,
    QMediaTypeAudio = 2,
    QMediaTypeSubtitle = 4
};

// Audio codec types
typedef NS_ENUM(NSInteger, QAudioCodec) {
    QAudioCodecUnknown = 0,
    QAudioCodecAAC,
    QAudioCodecOpus,
    QAudioCodecPcmA,
    QAudioCodecPcmU,
    QAudioCodecG722,
};

// Raw audio format
typedef NS_ENUM(NSInteger, QRawAudioFormat)  {
    QRawAudioFormatU8 = 0,
    QRawAudioFormatS16 = 1,
    QRawAudioFormatU16 = 2,
    QRawAudioFormatS32 = 3,
    QRawAudioFormatU32 = 4,
    QRawAudioFormatFLOAT = 5,
    QRawAudioFormatUnknown = 99
};

// Video codec types
typedef NS_ENUM(NSInteger, QVideoCodec) {
    QVideoCodecUnknown = 0,
    QVideoCodecVP8,
    QVideoCodecVP9,
    QVideoCodecMPEG4,
    QVideoCodecH264,
    QVideoCodecH265,
    QVideoCodecI420,
};

// Raw video format
typedef NS_ENUM(NSInteger, QRawVideoFormat) {
    QRawVideoFormatI420 = 0,
    QRawVideoFormatYV12 = 1,
    QRawVideoFormatYUY2 = 2,
    QRawVideoFormatUYVY = 3,
    QRawVideoFormatIYUV = 4,
    QRawVideoFormatARGB = 5,
    QRawVideoFormatRGB24 = 6,
    QRawVideoFormatRGB565 = 7,
    QRawVideoFormatARGB4444 = 8,
    QRawVideoFormatARGB1555 = 9,
    QRawVideoFormatMJPEG = 10,
    QRawVideoFormatNV12 = 11,
    QRawVideoFormatNV21 = 12,
    QRawVideoFormatBGRA = 13,
    QRawVideoFormatHardware = 98,
    QRawVideoFormatUnknown = 99
};

// enum for clockwise rotation.
typedef NS_ENUM(NSInteger, QVideoRotation) {
    QVideoRotation_0 = 0,
    QVideoRotation_90 = 90,
    QVideoRotation_180 = 180,
    QVideoRotation_270 = 270
};

@interface QVideoDescribe : NSObject
- (instancetype)initWithParamenters:(QVideoCodec)codec framerate:(float)framerate
                      width:(int)width height:(int)height bitrate:(int)bitrate;
@property (nonatomic, assign) QVideoCodec codec;
@property (nonatomic, assign) QRawVideoFormat rawFormat;
@property (nonatomic, assign) QVideoRotation rotation;
@property (nonatomic, assign) float framerate;
@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (nonatomic, assign) float bitrate;
@property (nonatomic, assign) int timeScale;

@end


@interface QAudioDescribe : NSObject
- (instancetype)initWithParamenters:(QAudioCodec)codec rawFormat:(QRawAudioFormat)rawFormat
                    samplerate:(float)samplerate nchannel:(int)nchannel bitrate:(int)bitrate;
@property (nonatomic, assign) QAudioCodec codec;
@property (nonatomic, assign) QRawAudioFormat rawFormat;
@property (nonatomic, assign) float samplerate;
@property (nonatomic, assign) int nchannel;
@property (nonatomic, assign) float bitrate;

@end

@interface QMediaDescribe : NSObject
@property (nonatomic, assign) QMediaType mediaType;
@property (nonatomic) QAudioDescribe* audioDescribe;
@property (nonatomic) QVideoDescribe* videoDescribe;
@end
