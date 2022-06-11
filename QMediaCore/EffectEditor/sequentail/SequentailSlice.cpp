//
//  SequentailSlice.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//


#include "SequentailSlice.h"

#define TIMD_DIFF 100

bool SequentailSlice::IsContinuous(const SequentailSlice& prev, const SequentailSlice& next) {
    if (prev.IsSamePath(next)) {
        if (abs(prev.GetSourceRannge()._end - next.GetSourceRannge()._start) < TIMD_DIFF) {
            return true;
        }
    }
    return false;
}

bool SequentailSlice::IsContain(int64_t time_ms) const {
    return (display_range_._start <= time_ms) && (display_range_._end > time_ms);
    
}

bool SequentailSlice::IsSamePath(const SequentailSlice& other) const {
    return !file_path_.compare(other.GetFilePath());
}

Range<int64_t> SequentailSlice::GetSourceRannge() const {
    if (!source_range_.isValid())
        source_range_ = Range<int64_t>(0, slice_duration_);
    return source_range_ ;
}
Range<int64_t> SequentailSlice::GetDisplayRannge() const {
    if (!display_range_.isValid()) {
        display_range_ = {GetSourceRannge()._start, GetActiveDisplayLength()};
    }
    return display_range_;
}

int64_t SequentailSlice::GetTargetDisplayPosition(int64_t time_ms) const  {
    auto active_range = GetDisplayRannge();
    if (time_ms < active_range._start ) return active_range._start;
    if (time_ms > active_range._start + GetActiveDisplayLength()) return active_range._start + GetActiveDisplayLength();
    return time_ms;
}

int64_t SequentailSlice::SourceTimeToDisplayTime(int64_t source_time) const {
    auto dsp_range = GetDisplayRannge();
    auto src_range = GetSourceRannge();
    if (source_time > src_range._end) {
        return dsp_range._end;
    }else {
        return dsp_range._start + (source_time - src_range._start) * time_scale_;
    }
}
int64_t SequentailSlice::DisplayTimeToSourceTime(int64_t display_time) const {
    auto display_range = GetDisplayRannge();
    auto src_range = GetSourceRannge();
    float src_distance = (float)(display_time - display_range._start) / time_scale_;
    return src_range._start + ((src_distance > src_range.getLength()) ? src_range.getLength() : src_distance);
}


int64_t SequentailSlice::GetActiveDisplayLength() const {
    return static_cast<long long>(GetSourceRannge().getLength() * time_scale_);
}
