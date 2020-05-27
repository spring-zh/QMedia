//
// Created by spring on 2020/5/19.
//

#ifndef QMEDIASDK_MEDIASOURCEADAPTER_H
#define QMEDIASDK_MEDIASOURCEADAPTER_H

#include "JniUtils.h"
#include "EffectEditor/MediaSource.h"
#include "VideoTargetAdapter.h"
#include "AudioTargetAdapter.h"

class MediaSourceAdapter : public MediaSource{
public:
    explicit MediaSourceAdapter(jobject jsource);
    ~MediaSourceAdapter();

    bool load() override;

    void unload() override;

    bool isInit() override;

    bool start(int64_t startMSec, int64_t endMSec) override ;
    void stop() override;

    bool seekTo(int64_t mSec) override;

    bool isEOF() override;

    VideoFrameDrawer *createVideoFrameDrawer() override;

    VideoFrame readNextVideoFrame(int &error) override;

    AudioFrame readNextAudioFrame(int &error) override;

protected:
    const JniUtils::JWeakObject _weakObj;
    std::unique_ptr<VideoTargetAdapter> _videoTargetAdapter;
    std::unique_ptr<AudioTargetAdapter> _audioTargetAdapter;
};


#endif //QMEDIASDK_MEDIASOURCEADAPTER_H
