//
//  SequentailDecoder.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//


#include "SegmentDecoder.h"
#include "Utils/Logger.h"

Decoder::UPtr CreateDecoder(const MediaDescribe& media_describe) {
    if (media_describe.mediatype_ == MediaType::Video) {
        return Decoder::UPtr(HardwareDecoderFactory::instance()->CreateDecoder(media_describe));
    } else if (media_describe.mediatype_ == MediaType::Audio) {
        return Decoder::UPtr(SoftwareDecoderFactory::instance()->CreateDecoder(media_describe));;
    } else return nullptr;
}

static bool IsSameVideoDescribe(const VideoDescribe& ths, const VideoDescribe& oth) {
    return (ths.codectype == oth.codectype &&
            ths.width == oth.width &&
            ths.height == oth.height);
}
static bool IsSameAudioDescribe(const AudioDescribe& ths, const AudioDescribe& oth) {
    return (ths.codectype == oth.codectype &&
            ths.samplerate == oth.samplerate &&
            ths.nchannel == oth.nchannel);
}
static bool IsSameMediaStreamDescribe(const MediaStreamDescribe& ths, const MediaStreamDescribe oth) {
    if (ths.describe.mediatype_ == MediaType::Video) {
        if (IsSameVideoDescribe(ths.describe._videodesc, oth.describe._videodesc)) {
            return ths.specificInfo.IsSameCodec(oth.specificInfo);
        }
    } else if (ths.describe.mediatype_ == MediaType::Audio) {
        if (IsSameAudioDescribe(ths.describe._audiodesc, oth.describe._audiodesc)) {
            return ths.specificInfo.IsSameCodec(oth.specificInfo);
        }
    }
    return false;
}

#define TIMD_DIFF 100
enum DecCmd : int {
    CMD_START = 1,
    CMD_SEEK,
    CMD_READ_DECODE,
    CMD_STOP
};

SegmentDecoder::SegmentDecoder(MediaType media_type, CallBack* callback, std::string name):
callback_(callback) , media_type_(media_type), thread_task_(name) {}

void SegmentDecoder::ReadDecode() {
    assert(thread_task_.IsCurrent());
    
    
    frame_cv_.Wait([this]()->bool {
        return !(is_runing_ && out_frames_.size() >= max_count_);
    });
    
    if (!is_runing_ || is_all_eof) return;

    int ret = __DecodeOnePacket();
    if(ret < 0 ) {
        callback_->OnSegmentDecodeError(ret);
    } else {
        // continue run ReadDecode
        thread_task_.PostTask(CMD_READ_DECODE, &SegmentDecoder::ReadDecode, this);
    }
}

std::future<bool> SegmentDecoder::StartMedia(std::vector<MediaSegmentImpl*>& segments, int64_t time_ms) {
    //TODO: merge continuous slices
    return thread_task_.PostTask(CMD_START, [this, &segments, time_ms]()->bool {
        segments_ = segments;
        if (segments.empty()) {
            return false;
        }
        return __SetPositionTo(time_ms, true);
    });
}

std::future<bool> SegmentDecoder::SetPositionTo(int64_t time_ms) {
    frame_cv_.Notify([this]()->bool{
        is_runing_ = false;
        return true;
    });
    return thread_task_.InsertTask(CMD_SEEK, [this, time_ms]()->bool {
        thread_task_.RemoveTask(CMD_READ_DECODE);
        return __SetPositionTo(time_ms);
    });
}

std::future<void> SegmentDecoder::StopMedia() {
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
DecodedFrame SegmentDecoder::ReadFrame(int64_t time_ms, int& index, MediaSegmentImpl** psegment) {
    DecodedFrame frame(nullptr, -1);
    index = -1;
    *psegment = NULL;
    frame_cv_.Wait([this, &index, psegment, time_ms, &frame](){
        if(!is_runing_) return true;
        index = GetSliceIndex(time_ms);
        if (index < 0) return true;
        
        *psegment = segments_[index];

        if (out_frames_.empty()) {
            return is_all_eof? true: false;
        }

        while (!out_frames_.empty() && (out_frames_.front().timestamp_ms_ <= time_ms)) {
            frame = out_frames_.front();
            out_frames_.pop_front();
            out_display_pos_ = frame.timestamp_ms_;
        }

        if (out_frames_.size() < max_count_) {
            frame_cv_.NotifyNoLock();
        }

        return true;
    });
    return frame;
}

int SegmentDecoder::GetUpperSliceIndex(int64_t time_ms) {
//    std::binary_search(slices_.begin(), slices_.end(), time_ms, [](int64_t time_ms, DecoderSlice& slice){
//        return (time_ms >= slice.GetSourceRannge()._start) && (time_ms < slice.GetSourceRannge()._end);
//    });
    
    int __first = 0;
    int __last = (int)segments_.size();
    
    while (__last - __first > 0) {
        int __half = (__last + __first) >> 1;
        if (segments_[__half]->getDisplayRange().end <= time_ms) {
            __first = __half + 1;
        } else if (segments_[__half]->getDisplayRange().start > time_ms) {
            __last = __half;
        } else
            return __half;
    }
    
    if (__last >= (int)segments_.size()) {
        __last = (int)segments_.size() - 1;
    }
    if (segments_[__last]->getDisplayRange().end < time_ms) {
        __last = -1;
    }
    return __last;
}

int SegmentDecoder::GetSliceIndex(int64_t time_ms) {
    int index = GetUpperSliceIndex(time_ms);
    if (index >=0 && !segments_[index]->IsContain(time_ms)) {
        index = -1;
    }
    return index;
}

#pragma mark implmention block function

int SegmentDecoder::__DecodeOnePacket() {
    // current slice is eof
    bool is_slice_eof = false;

    EncodedPacket pkt;
    int ret = demuxer_->ReadPacket(pkt);
    if (ret >= 0) {
        if (pkt.getEncodedBuffer()->stream_id() == in_stream_id) {
            //
            in_source_pos_ = pkt.ntp_time_ms();
            in_display_pos_ = segments_[in_slice_index_]->SourceTimeToDisplayTime(in_source_pos_);
            pkt.set_ntp_time_ms(in_display_pos_);
            if (!segments_[in_slice_index_]->IsContain(in_display_pos_)) {
                pkt.setFlags(pkt.flags() | EncodedPacket::FLAG_DISCARD);
            }
            if (in_display_pos_ >= segments_[in_slice_index_]->getDisplayRange().end) {
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
        if (in_slice_index_ + 1 < segments_.size()) {
            in_slice_index_ ++;
            if (!segments_[in_slice_index_-1]->IsContinuous(segments_[in_slice_index_])) {
                if (segments_[in_slice_index_-1]->IsSamePath(segments_[in_slice_index_])) {
                    in_display_pos_ = segments_[in_slice_index_]->getDisplayRange().start;
                    ret = demuxer_->Seek(segments_[in_slice_index_]->DisplayTimeToSourceTime(in_display_pos_), 0);
                }
                else {
                    ret = __OpenSlice(in_slice_index_, segments_[in_slice_index_]->getDisplayRange().start)? 0 : -1;
                }
            }
        } else {
            is_all_eof = true;
            decoder_->Flush(true);
            ret = -1;
        }
    }
    
    return ret;
}

bool SegmentDecoder::__DecodeFirstFrame() {
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

bool SegmentDecoder::__SetPositionTo(int64_t time_ms, bool is_first_open) {
    int index = GetUpperSliceIndex(time_ms);
    if (index < 0 || index >= segments_.size()) {
        return false;
    }
    
    // dec target time_ms
//    int64_t target_time_ms = MAX(time_ms, slices_[index].GetDisplayRannge()._start);
    int64_t target_time_ms = segments_[index]->GetTargetDisplayPosition(time_ms);
    bool need_decode = true;
    if (!is_first_open && !is_all_eof && (target_time_ms < in_display_pos_ + TIMD_DIFF * 5) && (target_time_ms > out_display_pos_ - TIMD_DIFF)) {
        //seek in cache
        LOGI("[seek] in cache out_display_pos_(%lld) target_time_ms(%lld) in_display_pos_(%lld)",
             out_display_pos_,
             target_time_ms,
             in_display_pos_);
        need_decode = frame_cv_.Run([this, target_time_ms]()->bool {
            position_limit_ = target_time_ms;
            out_frames_.remove_if([this](DecodedFrame& frame){
                return (frame.timestamp_ms_ < position_limit_ - TIMD_DIFF);
            });
            return out_frames_.empty();
        });
    } else {
        LOGI("[seek] fully target_time_ms(%lld)", target_time_ms);
        if (decoder_) {
            decoder_->Flush(true);
        }
        frame_cv_.Run([this, target_time_ms]() {
            out_display_pos_ = position_limit_ = target_time_ms;
            out_frames_.clear();
        });

        __OpenSlice(index, target_time_ms);
        is_all_eof = false;
    }
    
    is_runing_ = true;
    
    if (need_decode) {
        if(__DecodeFirstFrame()) {
            // start decode loop
            thread_task_.PostTask(CMD_READ_DECODE, &SegmentDecoder::ReadDecode, this);
            return true;
        } else {
            // time_ms doesn't in any slice
            return false;
        }
    }else {
        thread_task_.PostTask(CMD_READ_DECODE, &SegmentDecoder::ReadDecode, this);
        return true;
    }
    
    return false;
}

//open a decoder slice for reader and decoder
bool SegmentDecoder::__OpenSlice(int index, int64_t time_ms) {
    if (!demuxer_) {
        demuxer_ = Demuxer::UPtr(new GeneralDemuxer());
    }

    auto segment = segments_[index];
    demuxer_->Close();
    demuxer_->Open(segment->getFileName().c_str(), 0);
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
    
    demuxer_->Seek(segment->DisplayTimeToSourceTime(in_display_pos_), 0);
    
    auto& new_stream_desc = demuxer_->getMediaStreamDescribes()[in_stream_id];
    if (! IsSameMediaStreamDescribe(stream_dec_, new_stream_desc)) {
        //TODO: reset decoder
        LOGI("is not same codec info, reset decoder !!");
        stream_dec_ = new_stream_desc;
        if (decoder_) {
            decoder_->Release();
        }

        decoder_ = CreateDecoder(demuxer_->getMediaStreamDescribes()[in_stream_id].describe);
        decoder_->RegisterDecodedFrameCallback(this);
        int ret = decoder_->InitDecode(demuxer_->getMediaStreamDescribes()[in_stream_id].describe, 0, new_stream_desc.specificInfo);
        if(ret < 0) {
            LOGE("InitDecode Failed err : %d", ret);
            return false;
        }
    }
    return true;
}

#pragma mark implmention Decoder callback
int32_t SegmentDecoder::OnDecoded(DecodedFrame& decodedFrame) {
    if (decodedFrame.flags() & EncodedPacket::FLAG_DISCARD) {
        LOGI("decodedFrame flags FLAG_DISCARD");
        return false;
    }
    frame_cv_.Notify([this, &decodedFrame]()->bool {
        if (decodedFrame.timestamp_ms_ < position_limit_ - TIMD_DIFF) {
            LOGI("decodedFrame.timestamp_ms_(%lld) < position_limit_(%lld) - TIMD_DIFF",
                 decodedFrame.timestamp_ms_, position_limit_);
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
        if (out_display_pos_ == -1)out_display_pos_ = out_frames_.begin()->timestamp_ms_;
        if (out_max_pos_ < decodedFrame.timestamp_ms_) out_max_pos_ = decodedFrame.timestamp_ms_;
        return true;
    });
    return 0;
}

void SegmentDecoder::OnDecodedEnd() {
    
}
