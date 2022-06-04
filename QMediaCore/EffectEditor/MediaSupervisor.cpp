//
//  MediaSupervisor.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaSupervisor.h"
#include "MediaCore/decoder/SequentailDecoder.h"
//#include "MediaCore/core/PlayerClock.h"
//#include "Utils/Logger.h"

MediaSupervisor::MediaSupervisor() {
    sequentail_decoder_ = new SequentailDecoder();
    sequentail_decoder_->StopMedia();
}

MediaSupervisor::~MediaSupervisor() {
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
    timeRange._start = 0;
    return timeRange;
}

void MediaSupervisor::start()
{
    setPositionTo(0);
}

#define USE_ASYNC_CALL 1
void MediaSupervisor::setPositionTo(int64_t mSec)
{
    std::vector<std::future<bool>> futures;
    for (auto &track : _mediaTracks) {
        futures.push_back(track->setPositionTo(mSec));
    }
    for (auto& future : futures) {
        future.get();
    }
}

void MediaSupervisor::stop()
{
    std::vector<std::future<void>> futures;
    for (auto &track : _mediaTracks) {
        futures.push_back(track->stopMedia());
    }
    for (auto& future : futures) {
        future.get();
    }
}
