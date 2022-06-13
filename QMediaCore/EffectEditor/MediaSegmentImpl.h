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
#include "time_range.h"
#include "media_stream_info.h"
#include "media_segment.h"

namespace QMedia { namespace Api {

class MediaSegmentImpl : public MediaSegment {
public:
    explicit MediaSegmentImpl(const char* file_name);
    virtual ~MediaSegmentImpl();
    
    std::vector<MediaStreamInfo> getMediaStreamInfo() override;

    void enable(int32_t stream_id) override;

    std::string getFileName() override;

    void setTimescale(float time_scale) override;

    float getTimescale() override;

    void setSourceRange(const TimeRange & range) override;

    TimeRange getSourceRange() override;

    void setDisplayRange(const TimeRange & range) override;

    TimeRange getDisplayRange() override;

    int64_t getMediaDuration() override;
    
protected:
    int32_t Load(const char *urlpath, int flags);
    std::string file_name_;
    std::vector<MediaStreamInfo> media_streams_;
    int32_t enable_stream_id_ = -1;
    
    int64_t media_duration_ = 0;
    mutable std::experimental::optional<TimeRange> source_range_;
    mutable std::experimental::optional<TimeRange> display_range_;
    float time_scale_ = 1.0f;
};

}
}

#endif /* EFFECTEDITOR_MEDIASEGMENT_IMPL_H */
