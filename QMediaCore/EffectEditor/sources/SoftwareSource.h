//
//  SoftwareSource.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_SOFTWARESOURCE_H
#define EFFECTEDITOR_SOFTWARESOURCE_H

#include "EffectEditor/MediaSource.h"
#include "MediaCore/core/MediaStream.h"
#include "MediaCore/demuxer/Demuxer.h"

#define MAX_STREAMS 5

class SoftwareSource :public MediaSource{
public:
    explicit SoftwareSource(std::vector<std::string>& fileList, bool enableVideo = true, bool enableAudio = true) ;
    virtual ~SoftwareSource();

    virtual bool load() override ;
    virtual void unload() override ;
    virtual bool isInit() override ;

    virtual bool start(int64_t startMSec, int64_t endMSec) override ;
    virtual void stop() override ;
    virtual bool seekTo(int64_t mSec) override ;
    virtual bool isEOF() override ;
    
    virtual VideoFrameDrawer* createVideoFrameDrawer() override ;
    
    /**
     * @param error .succeed 0 , -1 system error , 1 stream end
     * this function is blocking while get frame or get error code
     */
    virtual VideoFrame readNextVideoFrame(int& error) override ;
    virtual AudioFrame readNextAudioFrame(int& error) override ;

protected:
    void run();
    /**
     * flush and decode media frame to timepoint mSec.
     * @param mSec millisecond
     * @param precise : false(seek to previous sync video frame)
     */
    bool decodeTo(int64_t mSec, bool precise = true);
private:
    int getDemuxerIndex(int64_t time_ms, int64_t& time_offset);
    bool _isLoaded;
    bool _enableVideo;
    bool _enableAudio;
    std::mutex _mutex;
    std::condition_variable _condition;
    std::vector<std::string> _fileList;
    std::vector<DemuxerRef> _demuxerList;

    std::vector<MediaStreamRef > _mediaStreams;
    int _stream_map[MAX_STREAMS];
    int _videoStreamIndex;
    int _audioStreamIndex;
    int _packetCacheConut;

    int _demuxerIndex;
    int64_t _duration_offset;
    std::thread _processThread;
};

#endif /* EFFECTEDITOR_SOFTWARESOURCE_H */
