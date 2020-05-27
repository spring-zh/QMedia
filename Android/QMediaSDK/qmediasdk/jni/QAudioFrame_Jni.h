//
// Created by spring on 2020/5/20.
//

#ifndef QMEDIASDK_QAUDIOFRAME_JNI_H
#define QMEDIASDK_QAUDIOFRAME_JNI_H

#include "JniUtils.h"
#include "MediaCore/mediadef/AudioFrame.h"

class JavaAudioFrameBuffer : public AudioFrameBuffer {
public:
    JavaAudioFrameBuffer(jobject jframe);
    ~JavaAudioFrameBuffer();
    int Channels() const override;

    int SampleRate() const override;

    int SampleBits() const override;

    int Samples() const override;

    const uint8_t *Data() const override;

    uint32_t Size() const override;

    enum ChannelLayout ChannelLayout() const override;

    std::shared_ptr<AudioFrameBuffer>
    ReSampleBuffer(RawAudioFormat fmt, int samplerate, int channels) override;
protected:
    jobject _object;
    uint8_t *_pbuffer;
    int64_t _size;
    int _channels;
    int _samplerate;
    int _samplebits;
};

extern AudioFrame createAudioFrameByObject(JNIEnv* env,jobject jframe);

#endif //QMEDIASDK_QAUDIOFRAME_JNI_H
