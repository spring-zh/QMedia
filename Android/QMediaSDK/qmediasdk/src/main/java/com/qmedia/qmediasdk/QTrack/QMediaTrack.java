package com.qmedia.qmediasdk.QTrack;

import android.util.Log;

import com.qmedia.qmediasdk.QAudio.QAudioTrackNode;
import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QEditor.QCombiner;
import com.qmedia.qmediasdk.QGraphic.QVideoTrackNode;
import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QSource.QMediaSource;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;

import java.util.UUID;

public class QMediaTrack {

    private static final String TAG = "QMediaTrack";

    public QMediaTrack(QMediaSource mediaSource) {
        this(mediaSource, UUID.randomUUID().toString());
    }

    public QMediaTrack(QMediaSource mediaSource, String id) {
        this.id = id;
        this.mediaSource = mediaSource;
        mPtr = native_create(mediaSource);
    }

    //construct this for children class
    protected QMediaTrack(String id) {
        this.id = id;
        this.mediaSource = null;
    }

    public String getId() {
        return id;
    }

    public QMediaSource getMediaSource() {
        return mediaSource;
    }

    public QVideoTrackNode getGraphic() {
        return graphic;
    }

    public QAudioTrackNode getAudio() {
        return audio;
    }

    public QVideoDescribe getVideoDescribe() {
        return mediaSource.getVideoDescribe();
    }
    public QAudioDescribe getAudioDescribe() {
        return mediaSource.getAudioDescribe();
    }

    public boolean prepare() {
        return native_prepare();
    }

    public boolean generateVideoTrackNode(QCombiner combiner) {
        if (mediaSource.getVideoDescribe() != null)
            graphic = new QVideoTrackNode(this, combiner);
        return graphic != null;
    }

    public boolean generateAudioTrackNode(QCombiner combiner) {
        if (mediaSource.getAudioDescribe() != null)
            audio = new QAudioTrackNode(this, combiner);
        return audio != null;
    }

    public boolean generateVideoTrackNode(QCombiner combiner, String id) {
        if (mediaSource.getVideoDescribe() != null)
            graphic = new QVideoTrackNode(this, combiner, id);
        return graphic != null;
    }

    public boolean setTimeTo(long mSec) {
        return native_setTimeTo(mSec);
    }
    public void stopMedia() {
        native_stopMedia();
    }

    public boolean isPrepare() {
        return native_isPrepare();
    }
    public float getPlaySpeed() {
        return native_getPlaySpeed();
    }
    public long getSourceDuration() {
        return native_getSourceDuration();
    }

    public QRange getDisplayRange(){
        return native_getDisplayRange();
    }
    public void setDisplayRange(QRange range){
        native_setDisplayRange(range);
    }
    public QRange getSourceRange(){
        return native_getSourceRange();
    }
    public void setSourceRange(QRange range){
        native_setSourceRange(range);
    }
    public float getTimeScale(){
        return native_getTimeScale();
    }
    public void setTimeScale(float timeScale){
        native_setTimeScale(timeScale);
    }
    public int getRepeatTimes(){
        return native_getRepeatTimes();
    }
    public void setRepeatTimes(int repeatTimes){
        native_setRepeatTimes(repeatTimes);
    }

    public void release() {
        native_release();
        mediaSource = null;
        mPtr = 0;
    }

    protected String id;
    protected QVideoTrackNode graphic;
    protected QAudioTrackNode audio;
    protected QMediaSource mediaSource;

    protected native long native_create(QMediaSource mediaSource);
    protected native boolean native_prepare();
    protected native boolean native_setTimeTo(long mSec);
    protected native void native_stopMedia();

    protected native boolean native_isPrepare();
    protected native float native_getPlaySpeed();
    protected native long native_getSourceDuration();

    protected native QRange native_getDisplayRange();
    protected native void native_setDisplayRange(QRange range);
    protected native QRange native_getSourceRange();
    protected native void native_setSourceRange(QRange range);
    protected native float native_getTimeScale();
    protected native void native_setTimeScale(float timeScale);
    protected native int native_getRepeatTimes();
    protected native void native_setRepeatTimes(int repeatTimes);

    protected native void native_release();

    //native ptr
    protected long mPtr = 0;
}
