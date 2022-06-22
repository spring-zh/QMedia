//
//  MediaSegmentManager.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaSegmentManager.h"
#include "medianode/VideoRenderNodeImpl.h"
#include "medianode/AudioRenderNodeImpl.h"
#include "Utils/Logger.h"
//#include "MediaCore/core/SteadyClock.h"
//#include "Utils/Logger.h"

//class TimeDurationPrint {
//public:
//    TimeDurationPrint(const char* logstr) {
//        logstring = logstr;
//        firstTimeMs = SystemClock::getCurrentTime<int64_t ,scale_milliseconds >();
//    }
//    ~TimeDurationPrint() {
//        int64_t secondTimeMs = SystemClock::getCurrentTime<int64_t ,scale_milliseconds >();
//        LOGI("%s Time Duration : %lld milliseconds", logstring.c_str(), (long long)secondTimeMs - firstTimeMs);
//    }
//    std::string logstring;
//    int64_t firstTimeMs;
//};

namespace QMedia { namespace Api {

MediaSegmentManager::MediaSegmentManager() {
}

MediaSegmentManager::~MediaSegmentManager() {
    video_ord_segments_.clear();
    audio_ord_segments_.clear();
}

bool MediaSegmentManager::RebuildSegmentTracks(SegmentTrack& ord_segments, std::vector<SegmentTrack>& segment_tracks) {
    std::vector<SegmentTrack> seg_tracks(1);
    seg_tracks[0].push_back(ord_segments[0]);
    for (int i = 1; i < ord_segments.size(); ++i) {
        bool is_overlap = true;
        for (auto& track : seg_tracks) {
            if (track[track.size()-1]->getDisplayRange().end <= ord_segments[i]->getDisplayRange().start) {
                track.push_back(ord_segments[i]);
                is_overlap = false;
                break;
            }
        }
        if (is_overlap) {
            seg_tracks.push_back(SegmentTrack());
            seg_tracks[seg_tracks.size()-1].push_back(ord_segments[i]);
        }
    }

    segment_tracks = seg_tracks;
    return true;
}

void MediaSegmentManager::ResetDecoders(int64_t position) {
    std::list<std::future<bool>> stop_futures;
    std::list<SegmentDecoder::UPtr> stop_decoders;
    if (video_track_segments_.size() != video_segment_decoders_.size()) {
        //TODO: reset video decoders
        while (video_track_segments_.size() < video_segment_decoders_.size()) {
//            auto decoder = std::move(video_segment_decoders_.back());
            stop_decoders.push_back( std::move(video_segment_decoders_.back()));
            video_segment_decoders_.pop_back();
            stop_futures.push_back(stop_decoders.back()->StopMedia());
        }
        while (video_track_segments_.size() > video_segment_decoders_.size()) {
            video_segment_decoders_.push_back(SegmentDecoder::UPtr(new SegmentDecoder(MediaType::Video, this)));

        }
    }
    
    for (int i = 0; i < video_segment_decoders_.size(); i++ ) {
        stop_futures.push_back(video_segment_decoders_[i]->StartMedia(video_track_segments_[i], position));
    }
    
    if (audio_track_segments_.size() != audio_segment_decoders_.size()) {
        //TODO: reset audio decoders
        while (audio_track_segments_.size() < audio_segment_decoders_.size()) {
//            auto decoder = std::move(audio_segment_decoders_.back());
            stop_decoders.push_back( std::move(audio_segment_decoders_.back()));
            audio_segment_decoders_.pop_back();
            stop_futures.push_back(stop_decoders.back()->StopMedia());
        }
        while (audio_track_segments_.size() > audio_segment_decoders_.size()) {
            audio_segment_decoders_.push_back(SegmentDecoder::UPtr(new SegmentDecoder(MediaType::Audio, this)));

        }
    }
    for (int i = 0; i < audio_segment_decoders_.size(); i++ ) {
        stop_futures.push_back(audio_segment_decoders_[i]->StartMedia(audio_track_segments_[i], position));
    }
    LOGI("ResetDecoders video_segment_decoders(%ld), audio_segment_decoders(%ld)",
         video_segment_decoders_.size(), audio_segment_decoders_.size());
    for (auto& fet : stop_futures) {
        fet.get();
    }
}

void MediaSegmentManager::addVideoSegment(const SEGMENT & segment) {
    video_ord_segments_.Insert((MediaSegmentImpl*)segment.get());
    video_ord_segments_.SortList();
    RebuildSegmentTracks(video_ord_segments_,video_track_segments_);
}
void MediaSegmentManager::removeVideoSegment(const std::shared_ptr<MediaSegment> & segment) {
    video_ord_segments_.Remove((MediaSegmentImpl*)segment.get());
    video_ord_segments_.SortList();
    RebuildSegmentTracks(video_ord_segments_,video_track_segments_);
}

void MediaSegmentManager::addAudioSegment(const std::shared_ptr<MediaSegment> & segment) {
    audio_ord_segments_.Insert((MediaSegmentImpl*)segment.get());
    audio_ord_segments_.SortList();
    RebuildSegmentTracks(audio_ord_segments_, audio_track_segments_);
}
void MediaSegmentManager::removeAudioSegment(const std::shared_ptr<MediaSegment> & segment) {
    audio_ord_segments_.Remove((MediaSegmentImpl*)segment.get());
    audio_ord_segments_.SortList();
    RebuildSegmentTracks(audio_ord_segments_, audio_track_segments_);
}

MediaRange MediaSegmentManager::getMediaTimeRange() const {
//    if (is_segment_update_) {
    MediaRange timeRange(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min());//{INT_MAX,-INT_MAX};
    for (auto &track : video_track_segments_){
        auto track_start = track[0]->getDisplayRange().start;
        auto track_end = track[track.size()-1]->getDisplayRange().end;
        if (timeRange.start > track_start) timeRange.start = track_start;
        if (timeRange.end < track_end) timeRange.end = track_end;
    }
    for (auto &track : audio_track_segments_){
        auto track_start = track[0]->getDisplayRange().start;
        auto track_end = track[track.size()-1]->getDisplayRange().end;
        if (timeRange.start > track_start) timeRange.start = track_start;
        if (timeRange.end < track_end) timeRange.end = track_end;
    }
    if (timeRange.start > timeRange.end) {
        timeRange = {0,0};
    }
    timeRange.start = 0;
        
    total_range_ = timeRange;
//        is_segment_update_ = false;
//    }
    
    return total_range_;
}

void MediaSegmentManager::start()
{
    ResetDecoders(0);
}

void MediaSegmentManager::setPositionTo(int64_t time_ms, bool update)
{
    if (update) {
        video_ord_segments_.SortList();
        audio_ord_segments_.SortList();
        RebuildSegmentTracks(video_ord_segments_, video_track_segments_);
        RebuildSegmentTracks(audio_ord_segments_, audio_track_segments_);
        ResetDecoders(time_ms);
    } else {
    std::vector<std::future<bool>> futures;
    for (auto &decoder : video_segment_decoders_) {
        futures.push_back(decoder->SetPositionTo(time_ms));
    }
    for (auto &decoder : audio_segment_decoders_) {
        futures.push_back(decoder->SetPositionTo(time_ms));
    }
    for (auto& future : futures) {
        future.get();
    }
    }
}

void MediaSegmentManager::stop() {
    std::vector<std::future<bool>> futures;
    for (auto &decoder : video_segment_decoders_) {
        futures.push_back(decoder->StopMedia());
    }
    for (auto &decoder : audio_segment_decoders_) {
        futures.push_back(decoder->StopMedia());
    }
    for (auto& future : futures) {
        future.get();
    }
}

void MediaSegmentManager::ReadVideoFrames(int64_t time_ms) {
    for (auto& v_decoder : video_segment_decoders_) {
        int index;
        MediaSegmentImpl *psegment;
        DecodedFrame vframe = v_decoder->ReadFrame(time_ms, index, &psegment);
        if (vframe.frame_buffer_ && psegment) {
            ((VideoRenderNodeImpl*)psegment->getVideo().get())->PutVideoFrame(vframe);
        }
    }
}

void MediaSegmentManager::ReadAudioFrames(int64_t time_ms) {
    for (auto& a_decoder : audio_segment_decoders_) {
        int index;
        MediaSegmentImpl *psegment;
        DecodedFrame aframe = a_decoder->ReadFrame(time_ms, index, &psegment);
        if (aframe.frame_buffer_ && psegment) {
            ((AudioRenderNodeImpl*)psegment->getAudio().get())->PutAudioFrame(aframe);
        }
    }
}

}
}
