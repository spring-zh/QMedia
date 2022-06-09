//
//  MediaComm.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <stdint.h>

enum class MediaType : int {Unkonw = 0,  Video = 1 , Audio = 2 , Subtitle = 4 };

// Raw video format
enum class RawVideoFormat {
	kI420 = 0,
	kYV12 = 1,
	kYUY2 = 2,
	kUYVY = 3,
	kIYUV = 4,
	kARGB = 5,
	kRGB24 = 6,
	kRGB565 = 7,
	kARGB4444 = 8,
	kARGB1555 = 9,
	kMJPEG = 10,
	kNV12 = 11,
	kNV21 = 12,
	kBGRA = 13,
	kHardware = 98,
	kUnknown = 99
};

// Video codec types
enum class VideoCodecType : int{
	kUnknown = 0,
	kVP8,
	kVP9,
	kMPEG4,
	kH264,
	kH265,
	kI420,

	MAX_NUM
};

// enum for clockwise rotation.
enum VideoRotation {
	kVideoRotation_0 = 0,
	kVideoRotation_90 = 90,
	kVideoRotation_180 = 180,
	kVideoRotation_270 = 270
};

struct VideoDescribe
{
	VideoCodecType codectype;
	RawVideoFormat format;
	VideoRotation rotation;
	float framerate;
	int width;
	int height;
	int bitrate;
	int timeScale;
};


// Raw audio format
enum class RawAudioFormat {
	kU8 = 0,
	kS16 = 1,
	kU16 = 2,
	kS32 = 3,
	kU32 = 4,
	kFLOAT = 5,
	kUnknown = 99
};

// Audio codec types
enum class AudioCodecType : int{
	kUnknown = 0,
	kAAC,
	kOpus,
	kPcmA,
	kPcmU,
	kG722,
    kAAC_LATM,
	kMP3,

    MAX_NUM
};

// TODO(mgraczyk): Remove once all methods that use ChannelLayout are gone.
enum ChannelLayout {
	kMono,
	// Left, right.
	kStereo,
	// Mono, keyboard, and mic.
	kMonoAndKeyboard,
	// Left, right, keyboard, and mic.
	kStereoAndKeyboard
};

struct AudioDescribe
{
	AudioCodecType codectype;
    RawAudioFormat format;
	int samplerate;
	int nchannel;
	int bitwidth;
	int bitrate;
};

struct SubtitleDescribe
{

};

struct MediaDescribe
{
	MediaType mediatype_;
	union 
	{
		VideoDescribe _videodesc;
		AudioDescribe _audiodesc;
		SubtitleDescribe _subtitledesc;
	};
};


//
#define UIN8_PTR(ptr, offset) ((uint8_t*)(ptr) + offset)
// memory read
#define MEM_RB8(ptr) (*UIN8_PTR(ptr,0))
#define MEM_RB16(ptr) (((uint16_t)MEM_RB8(ptr) <<8) | ((uint16_t)MEM_RB8(UIN8_PTR(ptr,1))))
#define MEM_RB24(ptr) (((uint32_t)MEM_RB16(ptr)<<8) | ((uint32_t)MEM_RB8(UIN8_PTR(ptr,2))))
#define MEM_RB32(ptr) (((uint32_t)MEM_RB16(ptr)<<16)| ((uint32_t)MEM_RB16(UIN8_PTR(ptr,2))))
#define MEM_RB64(ptr) (((uint64_t)MEM_RB32(ptr)<<32)| ((uint64_t)MEM_RB32(UIN8_PTR(ptr,4))))

// memory write
#define MEM_WB8(ptr, value) (*UIN8_PTR(ptr,0) = (value))
#define MEM_WB16(ptr, value) \
*UIN8_PTR(ptr,0) = (uint8_t)((value >> 8)&0xFF);\
*UIN8_PTR(ptr,1) = (uint8_t)((value     )&0xFF);

#define MEM_WB24(ptr, value) \
*UIN8_PTR(ptr,0) = (uint8_t)((value >> 16)&0xFF);\
*UIN8_PTR(ptr,1) = (uint8_t)((value >> 8)&0xFF);\
*UIN8_PTR(ptr,2) = (uint8_t)((value     )&0xFF);

#define MEM_WB32(ptr, value) \
*UIN8_PTR(ptr,0) = (uint8_t)((value >> 24)&0xFF);\
*UIN8_PTR(ptr,1) = (uint8_t)((value >> 16)&0xFF);\
*UIN8_PTR(ptr,2) = (uint8_t)((value >> 8)&0xFF);\
*UIN8_PTR(ptr,3) = (uint8_t)((value     )&0xFF);

#define MEM_WB64(ptr, value) \
*UIN8_PTR(ptr,0) = (uint8_t)((value >> 56)&0xFF);\
*UIN8_PTR(ptr,1) = (uint8_t)((value >> 48)&0xFF);\
*UIN8_PTR(ptr,2) = (uint8_t)((value >> 40)&0xFF);\
*UIN8_PTR(ptr,3) = (uint8_t)((value >> 32)&0xFF);\
*UIN8_PTR(ptr,4) = (uint8_t)((value >> 24)&0xFF);\
*UIN8_PTR(ptr,5) = (uint8_t)((value >> 16)&0xFF);\
*UIN8_PTR(ptr,6) = (uint8_t)((value >> 8)&0xFF);\
*UIN8_PTR(ptr,7) = (uint8_t)((value     )&0xFF);
