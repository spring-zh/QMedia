//
//  MediaSegmentImpl.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIASEGMENT_IMPL_H
#define EFFECTEDITOR_MEDIASEGMENT_IMPL_H

#include <memory>
#include "Utils/optional.hpp"
#include "media_range.h"
#include "media_stream_info.h"
#include "media_segment.h"

namespace QMedia { namespace Api {

class MediaSegmentImpl : public MediaSegment {
public:
    using SPtr = std::shared_ptr<MediaSegmentImpl>;
    using UPtr = std::unique_ptr<MediaSegmentImpl>;
    
    explicit MediaSegmentImpl(const char* file_name, int flag);
    virtual ~MediaSegmentImpl();
    
    std::shared_ptr<VideoRenderNode> getVideo() override;

    std::shared_ptr<AudioRenderNode> getAudio() override;
    
    std::vector<MediaStreamInfo> getMediaStreamInfo() override;
    
    bool isValid() override ;

    std::string getFileName() override;

    void setTimescale(float time_scale) override;

    float getTimescale() override;

    void setSourceRange(const MediaRange & range) override;

    MediaRange getSourceRange() override;

    void setDisplayRange(const MediaRange & range) override;

    MediaRange getDisplayRange() override;

    int64_t getMediaDuration() override;
    
    //
    // time remap
    int64_t SourceTimeToDisplayTime(int64_t source_time) ;
    int64_t DisplayTimeToSourceTime(int64_t display_time) ;
    
    int64_t GetTargetDisplayPosition(int64_t time_ms) ;
    
    bool IsContinuous(MediaSegmentImpl* next) ;
    bool IsSamePath(const MediaSegmentImpl* other) const;
    bool IsContain(int64_t time_ms) ;
    
protected:
    int32_t Load(const char *urlpath, int flags);
    int64_t GetActiveDisplayLength();
    std::string file_name_;
    bool is_valid_ = false;
    std::shared_ptr<VideoRenderNode> video_node_;
    std::shared_ptr<AudioRenderNode> audio_node_;
    std::vector<MediaStreamInfo> media_streams_;
    int32_t enable_stream_id_ = -1;
    
    int64_t media_duration_ = 0;
    mutable std::experimental::optional<MediaRange> source_range_;
    mutable std::experimental::optional<MediaRange> display_range_;
    float time_scale_ = 1.0f;
};

}
}

#endif /* EFFECTEDITOR_MEDIASEGMENT_IMPL_H */
