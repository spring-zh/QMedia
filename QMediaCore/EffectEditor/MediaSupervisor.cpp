//
//  MediaSupervisor.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaSupervisor.h"
#include <future>
#include <limits>
//#include "MediaCore/core/PlayerClock.h"
//#include "Utils/Logger.h"

MediaSupervisor::MediaSupervisor()
{
    
}

MediaSupervisor::~MediaSupervisor()
{
    _mediaTracks.clear();
}

Range<int64_t> MediaSupervisor::getMediaTimeRange() const
{
    Range<int64_t> timeRange(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min());//{INT_MAX,-INT_MAX};
    for (auto &track : _mediaTracks){
        Range<int64_t> tRange = track->getDisplayTrackRange();
        if (timeRange._start > tRange._start) {
            timeRange._start = tRange._start;
        }
        if (timeRange._end < tRange._end) {
            timeRange._end = tRange._end;
        }
    }
    if (timeRange._start > timeRange._end) {
        timeRange = {0,0};
    }
    return timeRange;
}

void MediaSupervisor::start()
{
    setPositionTo(0);
}

#define USE_ASYNC_CALL 1
void MediaSupervisor::setPositionTo(int64_t mSec)
{
#if USE_ASYNC_CALL
    //async call then sync
    if (_mediaTracks.size() > 1) {
        std::vector<std::future<bool>> futures;
        for (auto &track : _mediaTracks) {
//            std::packaged_task<bool(int64_t)> task(std::bind(&MediaTrack::setPositionTo, track));
            std::future<bool> fut= std::async(&MediaTrack::setPositionTo, track, mSec);
            futures.push_back(std::move(fut));
        }
        for (auto& future : futures) {
            future.get();
        }
    }else if (_mediaTracks.size() == 1){
        _mediaTracks.begin()->get()->setPositionTo(mSec);
    }
#endif
#if !USE_ASYNC_CALL
    //direct call
    for (auto &track : _mediaTracks) {
        track->setPositionTo(mSec);
    }
#endif
}

void MediaSupervisor::stop()
{
#if USE_ASYNC_CALL
    if (_mediaTracks.size() > 1) {
        std::vector<std::future<void>> futures;
        for (auto &track : _mediaTracks) {
//            std::packaged_task<void(void)> task(std::bind(&MediaTrack::stopMedia, track));
            std::future<void> fut= std::async(&MediaTrack::stopMedia, track);
            futures.push_back(std::move(fut));
        }
        for (auto& future : futures) {
            future.get();
        }
    }else if (_mediaTracks.size() == 1){
        _mediaTracks.begin()->get()->stopMedia();
    }
#endif
#if !USE_ASYNC_CALL
    for (auto &track : _mediaTracks) {
        track->stopMedia();
    }
#endif
}
