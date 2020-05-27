//
//  MediaComm.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

enum class MediaType { Video = 1 , Audio = 2 , Subtitle = 4 };

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
enum class VideoCodecType {
	kUnknown = 0,
	kVP8,
	kVP9,
	kMPEG4,
	kH264,
	kH265,
	kI420,
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
	float bitrate;
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
enum class AudioCodecType {
	kUnknown = 0,
	kAAC,
	kOpus,
	kPcmA,
	kPcmU,
	kG722,
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
	float bitrate;
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
