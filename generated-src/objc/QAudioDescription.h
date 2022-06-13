// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

#import <Foundation/Foundation.h>

@interface QAudioDescription : NSObject
- (nonnull instancetype)initWithSmapleFormat:(int16_t)smapleFormat
                                    nchannel:(int16_t)nchannel
                                  sampleRate:(int32_t)sampleRate
                                  sampleBits:(int32_t)sampleBits;
+ (nonnull instancetype)audioDescriptionWithSmapleFormat:(int16_t)smapleFormat
                                                nchannel:(int16_t)nchannel
                                              sampleRate:(int32_t)sampleRate
                                              sampleBits:(int32_t)sampleBits;

@property (nonatomic, readonly) int16_t smapleFormat;

@property (nonatomic, readonly) int16_t nchannel;

@property (nonatomic, readonly) int32_t sampleRate;

@property (nonatomic, readonly) int32_t sampleBits;

@end

extern int16_t const QAudioDescriptionFormatUnknow;
extern int16_t const QAudioDescriptionFormatU8;
extern int16_t const QAudioDescriptionFormatS16;
extern int16_t const QAudioDescriptionFormatU16;
extern int16_t const QAudioDescriptionFormatS32;
extern int16_t const QAudioDescriptionFormatU32;
extern int16_t const QAudioDescriptionFormatFloat;
extern int16_t const QAudioDescriptionFormatSamplebuffer;
extern int16_t const QAudioDescriptionFormatBytearray;
