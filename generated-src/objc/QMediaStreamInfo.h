// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#import "QAudioDescription.h"
#import "QVideoDescription.h"
#import <Foundation/Foundation.h>

@interface QMediaStreamInfo : NSObject
- (nonnull instancetype)initWithMediaType:(int32_t)mediaType
                                codecType:(int32_t)codecType
                                 duration:(int64_t)duration
                                  bitrate:(int64_t)bitrate
                                 streamId:(int32_t)streamId
                                     gops:(nonnull NSArray<NSNumber *> *)gops
                              timebaseNum:(int32_t)timebaseNum
                              timebaseDen:(int32_t)timebaseDen
                                    aDesc:(nonnull QAudioDescription *)aDesc
                                    vDesc:(nonnull QVideoDescription *)vDesc;
+ (nonnull instancetype)mediaStreamInfoWithMediaType:(int32_t)mediaType
                                           codecType:(int32_t)codecType
                                            duration:(int64_t)duration
                                             bitrate:(int64_t)bitrate
                                            streamId:(int32_t)streamId
                                                gops:(nonnull NSArray<NSNumber *> *)gops
                                         timebaseNum:(int32_t)timebaseNum
                                         timebaseDen:(int32_t)timebaseDen
                                               aDesc:(nonnull QAudioDescription *)aDesc
                                               vDesc:(nonnull QVideoDescription *)vDesc;

@property (nonatomic ) int32_t mediaType;

@property (nonatomic ) int32_t codecType;

@property (nonatomic ) int64_t duration;

@property (nonatomic ) int64_t bitrate;

@property (nonatomic ) int32_t streamId;

@property (nonatomic , nonnull) NSArray<NSNumber *> * gops;

/** timebase */
@property (nonatomic ) int32_t timebaseNum;

@property (nonatomic ) int32_t timebaseDen;

@property (nonatomic , nonnull) QAudioDescription * aDesc;

@property (nonatomic , nonnull) QVideoDescription * vDesc;

@end

/** media type */
extern int32_t const QMediaStreamInfoMediaUnknow;
extern int32_t const QMediaStreamInfoMediaVideo;
extern int32_t const QMediaStreamInfoMediaAudio;
/** codec type */
extern int32_t const QMediaStreamInfoCodecUnknow;
extern int32_t const QMediaStreamInfoCodecH264;
extern int32_t const QMediaStreamInfoCodecH265;
extern int32_t const QMediaStreamInfoCodecMpeg4;
extern int32_t const QMediaStreamInfoCodecVp8;
extern int32_t const QMediaStreamInfoCodecVp9;
extern int32_t const QMediaStreamInfoCodecMp3;
extern int32_t const QMediaStreamInfoCodecAac;
extern int32_t const QMediaStreamInfoCodecOpus;
extern int32_t const QMediaStreamInfoCodecPcma;
extern int32_t const QMediaStreamInfoCodecPcmu;