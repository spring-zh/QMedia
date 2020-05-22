package com.qmedia.qmediasdk.QEditor;

import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

public class QMediaFactory {

    QMediaFactory() {
        mPtr = native_create();
    }

    void setVideoTarget(QVideoTarget videoTarget, QCombiner combiner) {
        this.videoTarget = videoTarget;
        native_setVideoTarget(videoTarget, combiner);
    }
    void setAudioTarget(QAudioTarget audioTarget, QCombiner combiner) {
        this.audioTarget = audioTarget;
        native_setAudioTarget(audioTarget, combiner);
    }

    public QVideoTarget getVideoTarget() {
        return videoTarget;
    }

    public QAudioTarget getAudioTarget() {
        return audioTarget;
    }

    public void release() {
        native_release();
        videoTarget = null;
        audioTarget = null;
    }

    QVideoTarget videoTarget = null;
    QAudioTarget audioTarget = null;

    //TODO: native
    private native long native_create();
    private native void native_release();

    protected native void native_setVideoTarget(QVideoTarget videoTarget, QCombiner combiner);
    protected native void native_setAudioTarget(QAudioTarget audioTarget, QCombiner combiner);
    //native ptr
    private long mPtr = 0;
}
