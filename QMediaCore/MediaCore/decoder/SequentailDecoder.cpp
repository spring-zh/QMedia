//
//  SequentailDecoder.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//


#include "SequentailDecoder.h"
#include "Utils/Logger.h"

#define TIMD_DIFF 100

bool DecoderSlice::IsContinuous(const DecoderSlice& prev, const DecoderSlice& next) {
    if (!prev.GetFilePath().compare(next.GetFilePath())) {
        if (abs(prev.GetSourceRannge()._end - next.GetSourceRannge()._start) < TIMD_DIFF) {
            return true;
        }
    }
    return false;
}

enum DecCmd : int {
    CMD_START = 1,
    CMD_SEEK,
    CMD_READ_DECODE,
    CMD_STOP
};

int32_t SequentailDecoder::OnDecoded(DecodedFrame& decodedFrame) {
    frame_cv_.Notify([this, &decodedFrame]()->bool {
        if (decodedFrame.timestamp_ms_ < position_limit_) {
            return false;
        }
        // insert and sort
        auto iter = out_frames_.end();
        if (iter != out_frames_.begin()) {
            do {
                iter--;
                if (iter->timestamp_ms_ <= decodedFrame.timestamp_ms_) {
                    iter++;
                    break;
                }
            }while (iter != out_frames_.begin());
        }
        out_frames_.insert(iter, decodedFrame);
        
        //calculate out_display_pos
        out_display_pos_ = out_frames_.begin()->timestamp_ms_;
        if (out_max_pos_ < decodedFrame.timestamp_ms_) {
            out_max_pos_ = decodedFrame.timestamp_ms_;
        }
        return true;
    });
    return 0;
}

SequentailDecoder::SequentailDecoder(MediaType media_type, CallBack* callback, DecoderFactory* factory):
callback_(callback) , media_type_(media_type), dec_factory_(factory) {}

int SequentailDecoder::__DecodeOnePacket() {
    
    // current slice is eof
    bool is_slice_eof = false;

    EncodedPacket pkt;
    int ret = demuxer_->ReadPacket(pkt);
    if (ret >= 0) {
        if (pkt.getEncodedBuffer()->stream_id() == in_stream_id) {
            //
            in_source_pos_ = pkt.ntp_time_ms();
            in_display_pos_ = slices_[in_slice_index_].SourceTimeToDisplayTime(in_source_pos_);
            pkt.set_ntp_time_ms(in_display_pos_);
            if (!slices_[in_slice_index_].IsContain(in_display_pos_)) {
                pkt.setFlags(pkt.flags() | EncodedPacket::FLAG_DISCARD);
            }
            if (in_display_pos_ > slices_[in_slice_index_].GetDisplayRannge()._end) {
                is_slice_eof = true;
            }
            while ((ret = decoder_->Decode(pkt)) == DEC_EAGAIN && is_runing_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    } else {
        is_slice_eof = demuxer_->IsEOF();
        ret = 0;
    }

    
    if (is_slice_eof) { // current slice is eof,  read next slice
        if (in_slice_index_ + 1 < slices_.size()) {
            in_slice_index_ ++;
            if (!DecoderSlice::IsContinuous(slices_[in_slice_index_-1], slices_[in_slice_index_])) {
                return __OpenSlice(in_slice_index_, slices_[in_slice_index_].GetDisplayRannge()._start)? 0 : -1;
            }
        } else {
            is_all_eof = true;
            decoder_->Flush();
        }
    }
    
    return ret;
}

bool SequentailDecoder::__DecodeFirstFrame() {
    int index = GetSliceIndex(position_limit_);
    if (index < 0) {
        return false;
    }
    bool result = false;
    do {
        if(__DecodeOnePacket() < 0) break;
        result = frame_cv_.Run([this]()->bool {
            return (out_frames_.size() > 0);
        });
    } while(!result && is_runing_);
    return result;
}

bool SequentailDecoder::__SetPositionTo(int64_t time_ms, bool is_first_open) {
    int index = GetUpperSliceIndex(time_ms);
    if (index < 0) {
        return false;
    }
    
    // dec target time_ms
    int64_t target_time_ms = MAX(time_ms, slices_[index].GetDisplayRannge()._start);
    
    if (!is_first_open && (target_time_ms < in_display_pos_ + TIMD_DIFF * 5) && (target_time_ms > out_display_pos_ - TIMD_DIFF)) {
        //seek in cache
        frame_cv_.Run([this, time_ms]() {
            position_limit_ = time_ms;
            out_frames_.remove_if([this](DecodedFrame& frame){
                return (frame.timestamp_ms_ < position_limit_);
            });
            // set out_display_pos_
            if (out_frames_.empty()) {
                out_display_pos_ = out_max_pos_;
            }else
                out_display_pos_ = out_frames_.begin()->timestamp_ms_;
        });
    } else {
        frame_cv_.Run([this, time_ms]() {
            position_limit_ = time_ms;
            out_frames_.clear();
            out_display_pos_ = out_max_pos_;
        });

        __OpenSlice(index, target_time_ms);
        is_all_eof = false;
    }
    
    is_runing_ = true;
    
    if (target_time_ms == time_ms) {
        if(__DecodeFirstFrame()) {
            // start decode loop
            thread_task_.PostTask(CMD_READ_DECODE, &SequentailDecoder::ReadDecode, this);
            return true;
        }
    }else // time_ms doesn't in any slice
        return true;
    
    return false;
}

//open a decoder slice for reader and decoder
bool SequentailDecoder::__OpenSlice(int index, int64_t time_ms) {
    if (!demuxer_) {
        demuxer_ = Demuxer::UPtr(new GeneralDemuxer());
    }

    auto& slice = slices_[index];
    demuxer_->Close();
    demuxer_->Open(slice.GetFilePath().c_str(), 0);
    for (int i = 0; i < demuxer_->getMediaStreamDescribes().size(); ++i) {
        if (demuxer_->getMediaStreamDescribes()[i].describe.mediatype_ == media_type_) {
            in_stream_id = i;
            break;
        }
    }
    if (in_stream_id < 0) {
        LOGE("cannot find equal stream");
        return false;
    }
    demuxer_->Select(in_stream_id);
    
    in_slice_index_ = index;
    in_display_pos_ = time_ms;
    
    demuxer_->Seek(slices_[index].DisplayTimeToSourceTime(in_display_pos_), 0);
    
    auto& new_stream_desc = demuxer_->getMediaStreamDescribes()[in_stream_id];
    if (!stream_dec_.specificInfo.IsSameCodec(new_stream_desc.specificInfo)) {
        //TODO: reset decoder
        stream_dec_ = new_stream_desc;
        if (decoder_) {
            decoder_->Flush();
            decoder_->Release();
        }

        decoder_ = Decoder::UPtr( dec_factory_->CreateDecoder(demuxer_->getMediaStreamDescribes()[in_stream_id].describe));
        int ret = decoder_->InitDecode(demuxer_->getMediaStreamDescribes()[in_stream_id].describe, 0, new_stream_desc.specificInfo);
        if(ret < 0) {
            LOGE("InitDecode Failed err : %d", ret);
            return false;
        }
    }
    return true;
}

void SequentailDecoder::ReadDecode() {
    assert(thread_task_.IsCurrent());
    
    
    frame_cv_.Wait([this]()->bool {
        return !(is_runing_ && out_frames_.size() >= max_count_);
    });
    
    if (!is_runing_) return;

    int ret = __DecodeOnePacket();
    if(ret < 0 ) {
        callback_->OnError(ret);
    } else {
        // continue run ReadDecode
        thread_task_.PostTask(CMD_READ_DECODE, &SequentailDecoder::ReadDecode, this);
    }
}

std::future<bool> SequentailDecoder::StartMedia(std::vector<DecoderSlice>& slices, int64_t time_ms) {
    //TODO: merge continuous slices
    slices_ = slices;
    return thread_task_.PostTask(CMD_START, [this, &slices, time_ms]()->bool {
        return __SetPositionTo(time_ms, true);
    });
}

std::future<bool> SequentailDecoder::SetPositionTo(int64_t time_ms) {
    frame_cv_.Notify([this]()->bool{
        is_runing_ = false;
        return true;
    });
    return thread_task_.InsertTask(CMD_SEEK, [this, time_ms]()->bool {
        thread_task_.RemoveTask(CMD_READ_DECODE);
        return __SetPositionTo(time_ms);
    });
}

std::future<void> SequentailDecoder::StopMedia() {
    frame_cv_.Notify([this]()->bool{
        is_runing_ = false;
        return true;
    });
    
    return thread_task_.InsertTask(CMD_STOP, [this](){
        thread_task_.RemoveTask(CMD_READ_DECODE);
        if (decoder_) {
            decoder_->Release();
            decoder_.reset();
        }
        if (demuxer_) {
            demuxer_->Close();
            demuxer_.reset();
        }
        
        frame_cv_.Notify([this]()->bool{
            out_frames_.clear();
            position_limit_ = -1;
            out_display_pos_ = -1;
            out_max_pos_ = -1;
            return true;
        });
        in_source_pos_ = -1;
        in_display_pos_ = -1;
        in_slice_index_ = -1;
        out_slice_index_ = -1;
    });

}

//FIXME: when DecodedFrame is null, we can reuse last frame to render
DecodedFrame SequentailDecoder::ReadFrame(int64_t time_ms, int& index) {
    DecodedFrame frame(nullptr, -1);
    frame_cv_.Wait([this, &index, time_ms, &frame](){
        if(!is_runing_) return true;
        index = GetSliceIndex(time_ms);
        if (index < 0) return true;
        
        if (out_frames_.empty()) {
            return is_all_eof? true: false;
        }
        
        while (!out_frames_.empty() && (out_frames_.front().timestamp_ms_ < time_ms)) {
            frame = out_frames_.front();
            out_frames_.pop_front();
            out_display_pos_ = frame.timestamp_ms_;
        }

        return true;
    });
    return frame;
}

int SequentailDecoder::GetUpperSliceIndex(int64_t time_ms) {
//    std::binary_search(slices_.begin(), slices_.end(), time_ms, [](int64_t time_ms, DecoderSlice& slice){
//        return (time_ms >= slice.GetSourceRannge()._start) && (time_ms < slice.GetSourceRannge()._end);
//    });
    
    int __first = 0;
    int __last = (int)slices_.size();
    
    while (__last - __first > 0) {
        int __half = (__last - __first) >> 1;
        if (slices_[__half].GetDisplayRannge()._end <= time_ms) {
            __first = __half + 1;
        } else if (slices_[__half].GetDisplayRannge()._start > time_ms) {
            __last = __half;
        } else
            return __half;
    }
    
    if (slices_[__last].GetDisplayRannge()._end <= time_ms) {
        __last = -1;
    }
    return __last;
}

int SequentailDecoder::GetSliceIndex(int64_t time_ms) {
    int index = GetUpperSliceIndex(time_ms);
    if (index >=0 && !slices_[index].IsContain(time_ms)) {
        index = -1;
    }
    return index;
}
