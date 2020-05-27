package com.qmedia.qmediasdk.QSource;

import com.qmedia.qmediasdk.QAudio.QAudioFrame;
import com.qmedia.qmediasdk.QGraphic.QVideoFrame;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

public interface QMediaSource {

    void setVideoTarget(QVideoTarget videoTarget);
    void setAudioTarget(QAudioTarget audioTarget);
    QVideoTarget getVideoTarget();
    QAudioTarget getAudioTarget();

    QVideoDescribe getVideoDescribe();
    QAudioDescribe getAudioDescribe();

    boolean load();
    boolean unload();
    boolean isInit();
    boolean start(long startMSec, long endMSec);
    void stop();
    boolean seekTo(long mSec);
    boolean isEOF();
    boolean isStarted();
    long getMediaDuration();

    QVideoFrame readNextVideoFrame();
    QAudioFrame readNextAudioFrame();
}
