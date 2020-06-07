package com.qmedia.qmediasdk.QAudio;

import com.qmedia.qmediasdk.QEditor.QCombiner;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;

import java.lang.ref.WeakReference;

public class QAudioTrackNode {
    public QAudioTrackNode(QMediaTrack mediaTrack, QCombiner combiner){
        this.mediaTrack = mediaTrack;
        weakCombiner = new WeakReference<>(combiner);
        mPtr = native_create(mediaTrack);
        combiner.attachAudioNode(this, null);
    }

    public QMediaTrack getMediaTrack() {
        return mediaTrack;
    }

    public boolean isEnable() {
        return native_isEnable();
    }

    public void setEnable(boolean enable) {
        native_setEnable(enable);
    }

    public float getVolume() {
        return native_getVolume();
    }

    public void setVolume(float volume) {
        native_setVolume(volume);
    }

    public void release() {
        native_release();
        mediaTrack = null;
    }

    QMediaTrack mediaTrack;
    private WeakReference<QCombiner> weakCombiner = null;

    //TODO: native
    protected native long native_create(QMediaTrack mediaTrack);
    protected native void native_release();
    protected native boolean native_isEnable();
    protected native void native_setEnable(boolean enable);
    protected native float native_getVolume();
    protected native void native_setVolume(float volume);
    //native ptr
    private long mPtr = 0;
}
