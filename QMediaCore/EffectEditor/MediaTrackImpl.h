//
//  MediaTrackImpl.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIATRACKIMPL_H
#define EFFECTEDITOR_MEDIATRACKIMPL_H

#include <memory>
#include "Utils/rw_lock.h"
#include "MediaTrack.h"

class MediaTrackImpl : public MediaTrack {
public:
    explicit MediaTrackImpl(MediaSourceRef source);
    virtual ~MediaTrackImpl();
    
    const MediaSourceRef getMediaSource() const { return _sourceRef; }
    
    virtual bool prepare() override;
    
    virtual bool setPositionTo(int64_t mSec) override;//set read position to designated location
    virtual void stopMedia() override;//stop
    /**
     * timeScale(float) time scale for track, the equivalent of (1.0f / playe speed)
     * default 1.0f
     */
    virtual void setTimeScale(float timeScale) override { _timeScale = timeScale; }
    virtual float getTimeScale() const override { return _timeScale; }
    virtual float getPlaySpeed() const override;
    
    //set source repeat times ,default 1
    void setRepeatTimes(int repeatTimes) override { _repeatTimes = repeatTimes; }
    int getRepeatTimes() override { return _repeatTimes; }

    //get the absolute track range, if absolute track range not set then calculation it by paraments
    virtual const Range<int64_t> getDisplayTrackRange() const override;
    //set the real track area range for player and display |----|
    bool setDisplayTrackRange(Range<int64_t> absoluteRange) override {
        _absoluteTrackRange = absoluteRange;
        return true;
    }
    //set thre base source range, this is useful for split source data
    virtual bool setSourceRange(Range<int64_t> selectSourceRange) override ;
    virtual const Range<int64_t> getSourceRange() const override { return _sourceRange;}
    
    //map the track time to media source time
    virtual bool mapTimeToMediaSource(int64_t mSec, int64_t& outmSec) const;
    
    virtual bool isPrepare() const override { return _isPrepare; }
    
    virtual int64_t getMediaDuration() const override { return _sourceRef->getMediaDuration(); }
    
    virtual const VideoDescribe *const getVideoDescribe() const override;
    virtual const AudioDescribe *const getAudioDescribe() const override;
    virtual const VideoTarget* getVideoTarget() const override { return _sourceRef->getVideoTarget(); }
    virtual const AudioTarget* getAudioTarget() const override { return _sourceRef->getAudioTarget(); }
    
    virtual VideoFrameDrawer* createVideoFrameDrawer() override {return _sourceRef->createVideoFrameDrawer(); }
private:
    
    friend class MediaAudioChannel;
    friend class MediaGraphicChannel;
    /** function getVideoFrame getAudioFrame
     * @param mSec must use mapTimeToMediaSource before
     */
    virtual VideoFrame getVideoFrame(int64_t mSec , bool& isEnd) override;
    virtual AudioFrame getAudioFrame(int64_t mSec , bool& isEnd) override;
    
protected:
    
    MediaSourceRef _sourceRef;
    
    bool _isPrepare;
    int _repeatTimes;
    float _timeScale;
    Range<int64_t> _sourceRange; //customer select source range
    mutable Range<int64_t> _absoluteTrackRange;
    
    rwlock _rwlock;
    int64_t _last_video_ms;
    int64_t _last_audio_ms;
    int64_t _media_position_ms;
    
    int _video_stream_idx;
    int _audio_stream_idx;
};

#endif /* EFFECTEDITOR_MEDIATRACKIMPL_H */
