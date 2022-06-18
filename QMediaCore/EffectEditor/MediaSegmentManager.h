//
//  MediaSegmentManager.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIASEGMENT_MANAGER_H
#define EFFECTEDITOR_MEDIASEGMENT_MANAGER_H

#include <algorithm>
#include <vector>
#include "MediaSegmentImpl.h"
#include "MediaGraphicChannel.h"
#include "MediaAudioChannel.h"

#include "SegmentDecoder.h"

namespace QMedia { namespace Api {

using SEGMENT = std::shared_ptr<MediaSegment>;

class SegmentTrack : public std::vector<MediaSegmentImpl*> {
public:
    bool HasSegment(const MediaSegment* segment) {
        return std::find(begin(), end(), segment) != end();
    }
    
    bool Insert(MediaSegmentImpl* segment) {
        if (!HasSegment(segment)) {
            push_back(segment);
            return true;
        }
        return false;
    }
    
    bool Remove(const MediaSegmentImpl* segment) {
        auto iter = std::find(begin(), end(), segment);
        if (iter != end()) {
            erase(iter);
            return true;
        }
        return false;
    }
    
    void SortList() {
        std::sort(begin(), end(), [](MediaSegmentImpl* first, MediaSegmentImpl* second)->bool {
            return first->getDisplayRange().start < second->getDisplayRange().start;
        });
    }
};

struct SegComp {
    bool operator()(MediaSegmentImpl* first, MediaSegmentImpl* second) {
        return first->getDisplayRange().start < second->getDisplayRange().start;
    }
};

class MediaSegmentManager : public SegmentDecoder::CallBack {
public:
    MediaSegmentManager();
    virtual ~MediaSegmentManager();
    
    void addVideoSegment(const SEGMENT & segment);
    void removeVideoSegment(const SEGMENT & segment);
    
    void addAudioSegment(const SEGMENT & segment);
    void removeAudioSegment(const SEGMENT & segment);
    
    //proxy MediaSegment
    MediaRange getMediaTimeRange() const;
    
    void start();
    void setPositionTo(int64_t time_ms);
    void stop();
    
    void ReadVideoFrames(int64_t time_ms);
    void ReadAudioFrames(int64_t time_ms);
private:
    void OnSegmentDecodeError(int error) override {}
protected:
    bool RebuildSegmentTracks(SegmentTrack& ord_segments, std::vector<SegmentTrack>& segment_tracks);
    void ResetDecoders();
    SegmentTrack video_ord_segments_;
    SegmentTrack audio_ord_segments_;

    std::vector<SegmentTrack> video_track_segments_;
    std::vector<SegmentTrack> audio_track_segments_;
    
    std::vector<SegmentDecoder::UPtr> video_segment_decoders_;
    std::vector<SegmentDecoder::UPtr> audio_segment_decoders_;
    
    mutable bool is_segment_update_ = false;
    mutable MediaRange total_range_{0, 0};
};

}
}

#endif /* EFFECTEDITOR_MEDIASEGMENT_MANAGER_H */
