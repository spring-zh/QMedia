package com.qmedia.qmediasdk.QTarget;

import com.qmedia.qmediasdk.QSource.QAudioDescribe;

public interface QAudioTarget {
    boolean startAudio();
    boolean stopAudio();
    void pauseAudio();
    void resumeAudio();
    void flushAudio();
//for audio
    boolean initAudio(QAudioDescribe describe);
    void unInitAudio();
    int getSampleRate();
    int getChannels();
    int getFormat();
    void setVolume(float volume);
    float getVolume();

    void setAudioDelay(int audioDelay);
    int getAudioDelay();
    //render
    void setRender(QAudioRender audioRender);
    QAudioRender getRender();
}
