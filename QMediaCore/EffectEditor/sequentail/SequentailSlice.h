//
//  SequentailSlice.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_SEQUENCETAIL_SLICE_H
#define EFFECTEDITOR_SEQUENCETAIL_SLICE_H

#include <string>
#include "Utils/Comm.h"

class SequentailSlice {
public:
    SequentailSlice(std::string file_path, int64_t slice_duration) : file_path_(file_path), slice_duration_(slice_duration) {}
    ~SequentailSlice() = default;
    
    const std::string& GetFilePath() const { return file_path_; }
    int64_t GetMediaDuration() const { return slice_duration_; }
    Range<int64_t> GetSourceRannge() const ;
    Range<int64_t> GetDisplayRannge() const ;
    int64_t GetTargetDisplayPosition(int64_t time_ms) const ;
    
    static bool IsContinuous(const SequentailSlice& prev, const SequentailSlice& next) ;
    bool IsSamePath(const SequentailSlice& other) const;
    bool IsContain(int64_t time_ms) const ;
    
    void SetTimeScale(float time_scale) { time_scale_ = time_scale; }
    void SetSourceRannge(Range<int64_t> source_range) { source_range_ = source_range; }
    void SetDisplayRannge(Range<int64_t> display_range) { display_range_ = display_range; }
    void SetSourceRannge(int64_t start, int64_t end) { source_range_ = Range<int64_t>(start, end); }
    void SetDisplayRannge(int64_t start, int64_t end) { display_range_ = Range<int64_t>(start, end); }

    // time remap
    int64_t SourceTimeToDisplayTime(int64_t source_time) const ;
    int64_t DisplayTimeToSourceTime(int64_t display_time) const ;
    
private:
    int64_t GetActiveDisplayLength() const ;
    std::string file_path_;
    int64_t slice_duration_;
    mutable Range<int64_t> source_range_;
    mutable Range<int64_t> display_range_;
    float time_scale_ = 1.0f;
};

#endif //EFFECTEDITOR_SEQUENCETAIL_SLICE_H
