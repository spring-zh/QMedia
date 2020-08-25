package com.qmedia.qmediasdk.QAudio;

import com.qmedia.qmediasdk.QEditor.QCombiner;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;

import java.lang.ref.WeakReference;
import java.util.UUID;

public class QAudioTrackNode {
    public QAudioTrackNode(QMediaTrack mediaTrack, QCombiner combiner){
        this(mediaTrack,combiner, UUID.randomUUID().toString());
    }

    public QAudioTrackNode(QMediaTrack mediaTrack, QCombiner combiner, String uuid){
        this.id = uuid;
        this.mediaTrack = mediaTrack;
        weakCombiner = new WeakReference<>(combiner);
        mPtr = native_create(mediaTrack);
        combiner.addAudioNodeIndex(this);
        combiner.attachAudioNode(this, null);
    }

    public String getId() {
        return id;
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

    public float getPitch() {
        return native_getPitch();
    }

    public void setPitch(float pitch) {
        native_setPitch(pitch);
    }

    public void release() {
        native_release();
        mediaTrack = null;
    }

    protected String id;
    QMediaTrack mediaTrack;
    private WeakReference<QCombiner> weakCombiner = null;

    //TODO: native
    protected native long native_create(QMediaTrack mediaTrack);
    protected native void native_release();
    protected native boolean native_isEnable();
    protected native void native_setEnable(boolean enable);
    protected native float native_getVolume();
    protected native void native_setVolume(float volume);
    protected native float native_getPitch();
    protected native void native_setPitch(float pitch);
    //native ptr
    private long mPtr = 0;
}
