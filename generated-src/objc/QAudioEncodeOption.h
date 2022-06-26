// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_config.djinni

#import <Foundation/Foundation.h>

@interface QAudioEncodeOption : NSObject
- (nonnull instancetype)initWithCodectype:(int32_t)codectype
                                 nchannel:(int32_t)nchannel
                               samplerate:(int32_t)samplerate
                                  bitrate:(int32_t)bitrate
                             sampleformat:(int32_t)sampleformat;
+ (nonnull instancetype)audioEncodeOptionWithCodectype:(int32_t)codectype
                                              nchannel:(int32_t)nchannel
                                            samplerate:(int32_t)samplerate
                                               bitrate:(int32_t)bitrate
                                          sampleformat:(int32_t)sampleformat;

@property (nonatomic ) int32_t codectype;

@property (nonatomic ) int32_t nchannel;

@property (nonatomic ) int32_t samplerate;

@property (nonatomic ) int32_t bitrate;

/** see audio_description format */
@property (nonatomic ) int32_t sampleformat;

@end

/** codectype */
extern int32_t const QAudioEncodeOptionKUnknown;
extern int32_t const QAudioEncodeOptionKAAC;
extern int32_t const QAudioEncodeOptionKOpus;
extern int32_t const QAudioEncodeOptionKMPEG4;
extern int32_t const QAudioEncodeOptionKPcmA;
extern int32_t const QAudioEncodeOptionKPcmU;
extern int32_t const QAudioEncodeOptionKG722;
extern int32_t const QAudioEncodeOptionKAACLATM;
extern int32_t const QAudioEncodeOptionKMP3;