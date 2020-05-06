//
//  MediaTrack.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIATRACK_H
#define EFFECTEDITOR_MEDIATRACK_H

#include <stdint.h>
#include <memory>
#include "Utils/rw_lock.h"
#include "MediaSource.h"
#include "MediaGraphicChannel.h"
#include "MediaAudioChannel.h"

class MediaTrack {
public:
    
    virtual bool prepare() = 0;
    
    virtual bool setPositionTo(int64_t mSec) = 0;
    virtual void stopMedia() = 0;
    /**
     * timeScale(float) time scale for track, the equivalent of (1.0f / playe speed)
     * default 1.0f
     */
    virtual void setTimeScale(float timeScale) = 0;
    virtual float getTimeScale() const = 0;
    virtual float getPlaySpeed() const = 0;
    
    //set source repeat times ,default 1
    virtual void setRepeatTimes(int repeatTimes) = 0;
    virtual int getRepeatTimes() = 0;

    //set the real track area range for player and display |----|
    virtual bool setDisplayTrackRange(Range<int64_t> absoluteRange) = 0;
    //get the display track range, if display track range not set then calculation it by paraments
    virtual const Range<int64_t> getDisplayTrackRange() const = 0;
    
    //set thre base source range, this is useful for split source data
    virtual bool setSourceRange(Range<int64_t> selectSourceRange) = 0;
    //get the source track range, if source track range not set then return the media context's duration
    virtual const Range<int64_t> getSourceRange() const = 0;
    
    virtual bool isPrepare() const = 0;
    
    virtual int64_t getMediaDuration() const = 0;
    
    virtual MediaAudioChannelRef getMediaAudioChannel() = 0;
    virtual MediaGraphicChannelRef getMediaGraphicChannel() = 0;
    virtual const VideoDescribe *const getVideoDescribe() const = 0;
    virtual const AudioDescribe *const getAudioDescribe() const = 0;

    virtual VideoFrame getVideoFrame(int64_t mSec , bool& isEnd) = 0;
    virtual AudioFrame getAudioFrame(int64_t mSec , bool& isEnd) = 0;
    
    virtual const VideoTarget* getVideoTarget() const = 0;
    virtual const AudioTarget* getAudioTarget() const = 0;
    
    virtual VideoFrameDrawer* createVideoFrameDrawer() = 0;
};

using MediaTrackRef = std::shared_ptr<MediaTrack>;

#endif /* EFFECTEDITOR_MEDIATRACK_H */
