//
//  MediaSource.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIASOURCE_H
#define EFFECTEDITOR_MEDIASOURCE_H

#include "Utils/Comm.h"
#include "MediaCore/core/AudioFrame.h"
#include "MediaCore/core/VideoFrame.h"
#include "VideoFrameDrawer.h"
#include "MediaCore/output/VideoTarget.h"
#include "MediaCore/output/AudioTarget.h"

class MediaSource :public NonCopyable{
public:
    MediaSource():
    _videoTarget(nullptr),
    _audioTarget(nullptr),
    _media_time_range(0,0),
    _isStarted(false),
    _media_duration(0) {}
    virtual ~MediaSource() {}
    
    virtual void setVideoTarget(const VideoTarget* videoTarget){_videoTarget = videoTarget;}
    virtual void setAudioTarget(const AudioTarget* audioTarget){_audioTarget = audioTarget;}
    
    virtual const VideoTarget* getVideoTarget() const{ return _videoTarget; }
    virtual const AudioTarget* getAudioTarget() const{ return _audioTarget; }

    virtual std::string filePath() const = 0;
    virtual bool load() = 0; //init and get media info
    virtual void unload() = 0; //uninit
    virtual bool isInit() = 0;
    
    /**
     *
     * @param startMSec base start time
     * @param endMSec base end time
     * @param bRepleat read repleat
     * @return
     */
    virtual bool start(int64_t startMSec, int64_t endMSec) {
        _media_time_range._start = startMSec;
        _media_time_range._end = endMSec;
        return true;
    }
    virtual void stop() = 0;
    virtual bool seekTo(int64_t mSec) = 0;
    virtual bool isEOF() = 0;
    virtual bool isStarted() { return _isStarted; }
    virtual int64_t getMediaDuration() const {return _media_duration; }
    
    const std::vector<MediaDescribe>& getMediaDescribe() const {return _mediaDescs; }
    
    virtual VideoFrameDrawer* createVideoFrameDrawer() = 0;
    
    /**
     * @param error .succeed 0 , -1 system error , 1 stream end
     * this function is blocking while get frame or get error code
     */
    virtual VideoFrame readNextVideoFrame(int& error) = 0;
    virtual AudioFrame readNextAudioFrame(int& error) = 0;
    
protected:
    const VideoTarget* _videoTarget;
    const AudioTarget* _audioTarget;
    
    std::vector<MediaDescribe> _mediaDescs;
    
    Range<int64_t> _media_time_range;
    
    bool _isStarted;
    
    mutable int64_t _media_duration; //millisecond
};

CLASSREF(MediaSource)

#endif /* EFFECTEDITOR_MEDIASOURCE_H */
