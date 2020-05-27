package com.qmedia.qmediasdk.QAudio;

import java.nio.ByteBuffer;

public class QAudioFrame {
    public QAudioFrame(ByteBuffer buffer,int channel,int sampleRate,int samplesInFrame,int sampleBits,int size,long timestamp)
    {
        this.buffer = buffer;
        this.channel = channel;
        this.sampleRate = sampleRate;
        this.samplesInFrame = samplesInFrame;
        this.sampleBits = sampleBits;
        this.size = size;
        this.timestamp = timestamp;
    }
    int channels() {
        return channel;
    }
    int sampleRate() {
        return sampleRate;
    }
    int sampleBits() {
        return sampleBits;
    }
    int samples() {
        return samplesInFrame;
    }

    long timestamp() {
        return timestamp;
    }

    ByteBuffer buffer() {
        return buffer;
    }
    int Size() {
        return size;
    }

    private ByteBuffer buffer;
    private int channel;
    private int sampleRate;
    private int samplesInFrame;
    private int sampleBits;
    private int size;

    private long timestamp;
}
