//
//  MediaSegmentImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaSegmentImpl.h"

extern "C" {
#include "libavformat/avformat.h"
}

namespace QMedia { namespace Api {

MediaSegmentImpl::MediaSegmentImpl(const char* file_name) : file_name_(file_name) {
    
}
MediaSegmentImpl::~MediaSegmentImpl() {
    
}

std::vector<MediaStreamInfo> MediaSegmentImpl::getMediaStreamInfo() {
    return media_streams_;
}

void MediaSegmentImpl::enable(int32_t stream_id) {
}

std::string MediaSegmentImpl::getFileName() {
    return "";
}

void MediaSegmentImpl::setTimescale(float time_scale) {
    time_scale_ = time_scale;
}

float MediaSegmentImpl::getTimescale() {
    return time_scale_;
}

void MediaSegmentImpl::setSourceRange(const TimeRange & range) {
    source_range_ = range;
}

TimeRange MediaSegmentImpl::getSourceRange() {
    if (source_range_) return source_range_.value();
    return TimeRange(0, media_duration_);
}

void MediaSegmentImpl::setDisplayRange(const TimeRange & range) {
    display_range_ = range;
}

TimeRange MediaSegmentImpl::getDisplayRange() {
    if (display_range_) return display_range_.value();
    return getSourceRange();
}

int64_t MediaSegmentImpl::getMediaDuration() {
    return media_duration_;
}


int32_t MediaSegmentImpl::Load(const char *urlpath, int flags) {

    int ret = -1;
    file_name_ = urlpath;

    do {
        AVFormatContext *fmt_ctx = avformat_alloc_context();
        if (! fmt_ctx) {
            LOGE("avformat_alloc_context Could not allocate context.");
            break;
        }

        int err = avformat_open_input(&fmt_ctx, urlpath, NULL, NULL);
        if (err < 0) {
            avformat_free_context(_formatContext);
            _formatContext = NULL;
            LOGE("avformat_open_input %s error %d errstr: %s ", urlpath, err ,av_err2str(err));
            break;
        }

        err = avformat_find_stream_info(fmt_ctx, NULL);
        if (err < 0) {
            LOGE("avformat_find_stream_info %s error %d errstr: %s", urlpath, err, av_err2str(err));
        }

        media_streams_.clear();

        for (int i=0; i < _formatContext->nb_streams && i < 5; i++) {
            MediaStreamDescribe streamDescribe;
            if(!initMediaStreamDescribe(_formatContext->streams[i], streamDescribe)) {
                //TODO: discard unsupport stream
                _formatContext->streams[i]->discard = AVDISCARD_ALL;
            }
            _mediaStreamDescribes.push_back(streamDescribe);
        }
        
        ret = 0;
    }while (0);

    return ret;
}

}
}
