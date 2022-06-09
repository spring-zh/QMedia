//
//  SequentailDecoder.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef SEQUENCETAIL_DECODER_H
#define SEQUENCETAIL_DECODER_H

#include "Utils/Comm.h"
#include "Utils/ConditionVariable.h"
#include "Utils/ThreadTaskFuture.h"
#include "SoftwareDecoder.h"
#include "demuxer/GeneralDemuxer.h"

class DecoderSlice {
public:
    DecoderSlice(std::string file_path, int64_t media_file_duration) : file_path_(file_path), media_file_duration_(media_file_duration) {}
    ~DecoderSlice() = default;
    
    static bool IsContinuous(const DecoderSlice& prev, const DecoderSlice& next);
    
    const std::string& GetFilePath() const { return file_path_; }
    
    bool IsContain(int64_t time_ms) { return (display_range_._start <= time_ms) && (display_range_._end > time_ms); }
    
    void SetTimeScale(float time_scale) { time_scale_ = time_scale; }
    void SetSourceRannge(Range<int64_t> source_range) { source_range_ = source_range; }
    void SetDisplayRannge(Range<int64_t> display_range) { display_range_ = display_range; }
    Range<int64_t> GetSourceRannge() const {
        if (!source_range_.isValid())
            source_range_ = Range<int64_t>(0, media_file_duration_);
        return source_range_ ;
    }
    Range<int64_t> GetDisplayRannge() const {
        if (!display_range_.isValid())
            display_range_ = GetSourceRannge();
        return display_range_;
    }
    
    int64_t GetMediaDuration() const { return media_file_duration_; }
    
    int64_t SourceTimeToDisplayTime(int64_t source_time) const {
        float alpha = (float)(source_time - source_range_._start) / source_range_.getLength();
        auto display_range = GetDisplayRannge();
        return display_range._start + alpha * (display_range.getLength()) * time_scale_;
    }
    int64_t DisplayTimeToSourceTime(int64_t display_time) const {
        auto display_range = GetDisplayRannge();
        float alpha = (float)(display_time - display_range._start) / display_range.getLength() / time_scale_;
        return GetSourceRannge()._start + alpha * (GetSourceRannge().getLength());
    }
    
private:
    std::string file_path_;
    int64_t media_file_duration_;
    mutable Range<int64_t> source_range_;
    mutable Range<int64_t> display_range_;
    float time_scale_ = 1.0f;
};


class SequentailDecoder : public DecodedFrameCallback {
public:
    
    using UPtr = std::unique_ptr<SequentailDecoder>;
    using SPtr = std::shared_ptr<SequentailDecoder>;
    
    struct CallBack {
        virtual void OnSequentailError(int error) = 0;
    };

    SequentailDecoder(MediaType media_type, CallBack* callback, DecoderFactory* factory);
	virtual ~SequentailDecoder() {}
    
    void SetMaxOutputFrameCount(int max_count) { max_count_ = max_count; }

    virtual std::future<bool> StartMedia(std::vector<DecoderSlice>& slices, int64_t time_ms = 0);
    
    virtual std::future<bool> SetPositionTo(int64_t time_ms);//set read position to designated location
    
    virtual std::future<void> StopMedia();//stop
    
    virtual DecodedFrame ReadFrame(int64_t time_ms, int& index);
    
private:
    int32_t OnDecoded(DecodedFrame& decodedFrame) override;
    void OnDecodedEnd() override;
    //
    int GetUpperSliceIndex(int64_t time_ms);
    int GetSliceIndex(int64_t time_ms);
    void ReadDecode();
protected:
    int __DecodeOnePacket();
    bool __DecodeFirstFrame();
    bool __SetPositionTo(int64_t time_ms, bool is_first_open = false);
    bool __OpenSlice(int index, int64_t time_ms);
    
    MediaType media_type_;
    CallBack* callback_;
    ThreadTaskFuture thread_task_;
    volatile bool is_runing_ = false;
    
    /** decoder slices
     * It has to be ordered and discontinuous, and it doesn't overlap
     */
    std::vector<DecoderSlice> slices_;
    Demuxer::UPtr demuxer_;
    int in_stream_id = -1;
    int in_slice_index_ = -1;
    int out_slice_index_ = -1;
    int64_t in_display_pos_ = -1;
    int64_t out_display_pos_ = -1;
    int64_t out_max_pos_ = -1;
    
    int64_t in_source_pos_ = -1;
    
    // decode
    DecoderFactory* dec_factory_;
    MediaStreamDescribe stream_dec_;
    Decoder::UPtr decoder_;
    int64_t position_limit_ = -1;
    
    // frame
    ConditionVariable frame_cv_;
    std::list<DecodedFrame> out_frames_;
    bool is_all_eof = false;
    
    int max_count_ = 4;
};


#endif //SEQUENCETAIL_DECODER_H
