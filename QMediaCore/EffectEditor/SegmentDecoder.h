//
//  SequentailDecoder.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_SEQUENCETAIL_DECODER_H
#define EFFECTEDITOR_SEQUENCETAIL_DECODER_H

#include "Utils/Comm.h"
#include "Utils/ConditionVariable.h"
#include "Utils/ThreadTaskFuture.h"
#include "MediaSegmentImpl.h"
#include "MediaCore/decoder/Decoder.h"
#include "MediaCore/demuxer/GeneralDemuxer.h"

using MediaSegmentImpl = QMedia::Api::MediaSegmentImpl;

class SegmentDecoder : public DecodedFrameCallback {
public:
    
    using UPtr = std::unique_ptr<SegmentDecoder>;
    using SPtr = std::shared_ptr<SegmentDecoder>;
    
    struct CallBack {
        virtual void OnSegmentDecodeError(int error) = 0;
    };

    SegmentDecoder(MediaType media_type, CallBack* callback, std::string name = "");
	virtual ~SegmentDecoder() {}
    
    void SetMaxOutputFrameCount(int max_count) { max_count_ = max_count; }

    virtual std::future<bool> StartMedia(std::vector<MediaSegmentImpl*>& segments, int64_t time_ms = 0);
    
    virtual std::future<bool> SetPositionTo(int64_t time_ms);//set read position to designated location
    
    virtual std::future<bool> StopMedia();//stop
    
    virtual DecodedFrame ReadFrame(int64_t time_ms, int& index, MediaSegmentImpl** psegment);
    
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
    std::vector<MediaSegmentImpl*> segments_;
    Demuxer::UPtr demuxer_;
    int in_stream_id = -1;
    int in_slice_index_ = -1;
    int out_slice_index_ = -1;
    int64_t in_display_pos_ = -1;
    int64_t out_display_pos_ = -1;
    int64_t out_max_pos_ = -1;
    
    int64_t in_source_pos_ = -1;
    
    // decode
    MediaStreamDescribe stream_dec_;
    Decoder::UPtr decoder_;
    int64_t position_limit_ = -1;
    
    // frame
    ConditionVariable frame_cv_;
    std::list<DecodedFrame> out_frames_;
    bool is_all_eof = false;
    
    int max_count_ = 4;
};


#endif //EFFECTEDITOR_SEQUENCETAIL_DECODER_H
