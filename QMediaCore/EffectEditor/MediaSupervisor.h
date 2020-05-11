//
//  MediaSupervisor.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIASUPERVISOR_H
#define EFFECTEDITOR_MEDIASUPERVISOR_H

#include <set>
#include <vector>
#include "MediaTrack.h"
#include "MediaGraphicChannel.h"
#include "MediaAudioChannel.h"

class MediaSupervisor {
public:
    MediaSupervisor();
    ~MediaSupervisor();
    
    inline bool hasMediaTrack(const MediaTrackRef& mediaTrack) const {
        return _mediaTracks.find(mediaTrack) != _mediaTracks.end();
    }
    
    void setPlayerTimeRange(Range<int64_t> range) { _playerTimeRange = range; }
    const Range<int64_t>& getPlayerTimeRange() const { return _playerTimeRange; }
    
    //proxy mediaTracks
    Range<int64_t> getMediaTimeRange() const;
    
    void start();
    void setPositionTo(int64_t mSec);
    void stop();
    
protected:
    
    Range<int64_t> _playerTimeRange;
    std::set<MediaTrackRef> _mediaTracks;
    
    std::list<GraphicCore::RenderNodeRef> _graphicChannels;
    std::list<MediaAudioChannelRef> _audioChannels;
};

#endif /* EFFECTEDITOR_MEDIASUPERVISOR_H */
