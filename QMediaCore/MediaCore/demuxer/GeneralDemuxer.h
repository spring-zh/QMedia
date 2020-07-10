//
//  GeneralDemuxer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "Demuxer.h"

struct AVFormatContext;
struct AVStream;

class GeneralDemuxer : public Demuxer {
public:
    GeneralDemuxer();
    virtual ~GeneralDemuxer();

    virtual int32_t Open(const char *urlpath, int flags) override ;

    virtual int32_t Close() override ;

    virtual const std::vector <MediaStreamDescribe> &getMediaStreamDescribes() const override { return _mediaStreamDescribes; }

    virtual int32_t ReadPacket(EncodedPacket& packet) override;

    virtual int32_t Seek(int64_t seek_time, int flags = 0) override;

    virtual int32_t Flush() override;

    virtual bool IsOpen() const override { return _isOpen; }

    virtual bool IsEOF() const override { return _isEOF; }

    virtual void setIgnoreBFrame(bool ignore) override ;

    virtual int64_t getDuration() const override ;

protected:
    bool initMediaStreamDescribe(AVStream* avstream, MediaStreamDescribe& mediaStreamDescribe);

    bool _isOpen;
    bool _isEOF;
    std::string _url;
    AVFormatContext * _formatContext;
    std::vector <MediaStreamDescribe> _mediaStreamDescribes;
};
