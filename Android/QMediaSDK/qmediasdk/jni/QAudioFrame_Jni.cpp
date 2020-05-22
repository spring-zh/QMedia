//
// Created by spring on 2020/5/20.
//

#include "QAudioFrame_Jni.h"
#include "j4a_generate/j4a_generate.h"

JavaAudioFrameBuffer::JavaAudioFrameBuffer(jobject jframe) {
    JNIEnv* env = JniUtils::getEnv();
    _object = env->NewWeakGlobalRef(jframe);
    jobject jbuffer = J4AC_com_qmedia_qmediasdk_QAudio_QAudioFrame__buffer(env, jframe);
    _pbuffer = (uint8_t*)env->GetDirectBufferAddress(jbuffer);
    _size = env->GetDirectBufferCapacity(jbuffer);
    _channels = J4AC_com_qmedia_qmediasdk_QAudio_QAudioFrame__channels(env, jframe);
    _samplerate = J4AC_com_qmedia_qmediasdk_QAudio_QAudioFrame__sampleRate(env, jframe);
    _samplebits = J4AC_com_qmedia_qmediasdk_QAudio_QAudioFrame__sampleBits(env, jframe);
}
JavaAudioFrameBuffer::~JavaAudioFrameBuffer() {
    JNIEnv* env = JniUtils::getEnv();
    env->DeleteWeakGlobalRef(_object);
}

int JavaAudioFrameBuffer::Channels() const {
    return _channels;
}

int JavaAudioFrameBuffer::SampleRate() const {
    return _samplerate;
}

int JavaAudioFrameBuffer::SampleBits() const {
    return _samplebits;
}

int JavaAudioFrameBuffer::Samples() const {
    return _size * 8 / (_channels * _samplebits);
}

const uint8_t *JavaAudioFrameBuffer::Data() const {
    return _pbuffer;
}

uint32_t JavaAudioFrameBuffer::Size() const {
    return _size;
}

enum ChannelLayout JavaAudioFrameBuffer::ChannelLayout() const {
    enum ChannelLayout channellayout = kMono;
    if (_channels == 2)
        channellayout = kStereo;
    return channellayout;
}

std::shared_ptr<AudioFrameBuffer>
JavaAudioFrameBuffer::ReSampleBuffer(RawAudioFormat fmt, int samplerate, int channels) {
    return nullptr;
}


AudioFrame createAudioFrameByObject(JNIEnv* env,jobject jframe) {
    std::shared_ptr<AudioFrameBuffer> jvideoframebuffer = std::shared_ptr<AudioFrameBuffer>(new JavaAudioFrameBuffer(jframe));
    jlong timestamp = J4AC_com_qmedia_qmediasdk_QAudio_QAudioFrame__timestamp(env, jframe);
    return AudioFrame(jvideoframebuffer, timestamp, timestamp);
}
