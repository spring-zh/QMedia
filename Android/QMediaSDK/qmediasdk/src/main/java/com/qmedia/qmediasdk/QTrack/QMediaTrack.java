package com.qmedia.qmediasdk.QTrack;

import android.util.Log;

import com.qmedia.qmediasdk.QAudio.QAudioTrackNode;
import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QEditor.QCombiner;
import com.qmedia.qmediasdk.QGraphic.QVideoTrackNode;
import com.qmedia.qmediasdk.QSource.QMediaSource;

public class QMediaTrack {

    private static final String TAG = "QMediaTrack";

    public QMediaTrack(QMediaSource mediaSource) {
        this.mediaSource = mediaSource;
        mPtr = native_create(mediaSource);
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

    public boolean prepare(QCombiner combiner) {
        if (native_prepare()) {
            if (mediaSource.getVideoDescribe() != null)
                graphic = new QVideoTrackNode(this, combiner);
            if (mediaSource.getAudioDescribe() != null)
                audio = new QAudioTrackNode(this, combiner);
            return true;
        }else {
            Log.e(TAG, "QMediaTrack prepare failed..");
            return false;
        }
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
    private long mPtr = 0;
}
