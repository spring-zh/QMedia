//
// Created by spring on 2020/5/19.
//

#ifndef QMEDIASDK_AUDIOTARGETADAPTER_H
#define QMEDIASDK_AUDIOTARGETADAPTER_H

#include "JniUtils.h"
#include "MediaCore/output/AudioTarget.h"

class AudioTargetAdapter : public AudioTarget{
public:
    AudioTargetAdapter(jobject jtarget) ;
    ~AudioTargetAdapter() ;
    bool init(AudioDescribe audioDesc) override;

    void unInit() override;

    int getSampleRate() const override;

    int getChannels() const override;

    RawAudioFormat getFormat() const override;

    void setVolume(float volume) override;

    float getVolume() const override;

    int getAudioDelayBytes() const override;

    bool start() override;

    bool stop() override;

    void pause(bool isPause) override;

    void flush() override;

protected:
    jobject _object;
};


#endif //QMEDIASDK_AUDIOTARGETADAPTER_H
