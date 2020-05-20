//
// Created by spring on 2020/5/19.
//

#ifndef QMEDIASDK_MEDIASOURCEADAPTER_H
#define QMEDIASDK_MEDIASOURCEADAPTER_H

#include "JniUtils.h"
#include "EffectEditor/MediaSource.h"

class MediaSourceAdapter : public MediaSource{
public:
    MediaSourceAdapter(jobject jsource);
    ~MediaSourceAdapter();

    bool load() override;

    void unload() override;

    bool isInit() override;

    void stop() override;

    bool seekTo(int64_t mSec) override;

    bool isEOF() override;

    VideoFrameDrawer *createVideoFrameDrawer() override;

    VideoFrame readNextVideoFrame(int &error) override;

    AudioFrame readNextAudioFrame(int &error) override;

protected:
    jobject _object;
};


#endif //QMEDIASDK_MEDIASOURCEADAPTER_H
