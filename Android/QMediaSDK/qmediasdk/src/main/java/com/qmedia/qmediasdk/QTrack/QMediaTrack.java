package com.qmedia.qmediasdk.QTrack;

import android.util.Log;

import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QSource.QMediaSource;

public class QMediaTrack {

    private static final String TAG = "QMediaTrack";

    public QMediaTrack(QMediaSource mediaSource) {
        this.mediaSource = mediaSource;
        mPtr = native_create(mediaSource);
        boolean bRet = native_prepare();
        if (bRet) {
            Log.e(TAG, "QMediaTrack prepare failed..");
        }
    }
    boolean prepare() {
        return native_prepare();
    }
    boolean setTimeTo(long mSec) {
        return native_setTimeTo(mSec);
    }
    void stopMedia() {
        native_stopMedia();
    }

    boolean isPrepare() {
        return native_isPrepare();
    }
    float getPlaySpeed() {
        return native_getPlaySpeed();
    }
    long getSourceDuration() {
        return native_getSourceDuration();
    }

    QRange getDisplayRange(){
        return native_getDisplayRange();
    }
    void setDisplayRange(QRange range){
        native_setDisplayRange(range);
    }
    QRange getSourceRange(){
        return native_getSourceRange();
    }
    void setSourceRange(QRange range){
        native_setSourceRange(range);
    }
    float getTimeScale(){
        return native_getTimeScale();
    }
    void setTimeScale(float timeScale){
        native_setTimeScale(timeScale);
    }
    int getRepeatTimes(){
        return native_getRepeatTimes();
    }
    void setRepeatTimes(int repeatTimes){
        native_setRepeatTimes(repeatTimes);
    }

    void release() {
        native_release();
        mediaSource = null;
        mPtr = 0;
    }

    public QMediaSource mediaSource;

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
