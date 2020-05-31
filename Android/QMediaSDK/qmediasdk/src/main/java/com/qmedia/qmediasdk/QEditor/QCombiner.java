package com.qmedia.qmediasdk.QEditor;

import android.util.Log;

import com.qmedia.qmediasdk.QAudio.QAudioTrackNode;
import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QGraphic.QGraphicNode;
import com.qmedia.qmediasdk.QMediaSDK;
import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;

import java.util.ArrayList;

public class QCombiner extends QMediaFactory{
    private static final String TAG = "QCombiner";

    protected DisplayRootNode rootNode = new DisplayRootNode();

    protected QCombiner() {
        super.setCombiner(this);
        Log.i(TAG, "QMediaSDK Laod version " + QMediaSDK.SDK_VERSION);
    }

    protected void setVideoTarget(QVideoTarget videoTarget) {
        super.videoTarget = videoTarget;
        native_setVideoTarget(videoTarget);
    }

    protected void setAudioTarget(QAudioTarget audioTarget) {
        super.audioTarget = audioTarget;
        native_setAudioTarget(audioTarget);
    }

    public void setVideoConfig(QVideoDescribe describe) {
        native_setVideoConfig(describe);
    }
    public void setAudioConfig(QAudioDescribe describe) {
        native_setAudioConfig(describe);
    }

    public DisplayRootNode getRootNode() {
        return rootNode;
    }

    //TODO: mediaTracks @QMediaTrack
    public ArrayList<QMediaTrack> getMediaTracks() {
        return mediaTracks;
    }
    public boolean addMediaTrack(QMediaTrack track) {
        boolean bRet = super.addMediaTrack(track);
        if (bRet) {
            native_addMediaTrack(track);
            if (track.getAudio() != null)
                native_attachAudioNode(track.getAudio(), null);
        }
        return bRet;
    }
    public boolean removeMediaTrack(QMediaTrack track) {
        boolean bRet = super.removeMediaTrack(track);
        if (bRet) {
            native_removeMediaTrack(track);
            if (track.getAudio() != null)
                native_detachAudioNode(track.getAudio());
        }
        return bRet;
    }

    public boolean attachRenderNode(QGraphicNode child, QGraphicNode parent) {
        return native_attachRenderNode(child,parent);
    }

    public boolean detachRenderNode(QGraphicNode current) {
        return native_detachRenderNode(current);
    }

    public long getPosition() {
        return native_getPosition();
    }

    public QRange getMediaTimeRange() {
        return native_getMediaTimeRange();
    }


    public class DisplayRootNode extends QGraphicNode {
        DisplayRootNode() {
            super(QCombiner.this);
        }

        public void setBKColor(QVector color) {
            setColor4(color);
        }
    }

    //TODO: native
    protected native void native_setVideoConfig(QVideoDescribe describe);
    protected native void native_setAudioConfig(QAudioDescribe describe);
    protected native void native_setVideoTarget(QVideoTarget videoTarget);
    protected native void native_setAudioTarget(QAudioTarget audioTarget);
    protected native void native_addMediaTrack(QMediaTrack track);
    protected native void native_removeMediaTrack(QMediaTrack track);
    protected native boolean native_attachRenderNode(QGraphicNode child, QGraphicNode parent);
    protected native boolean native_detachRenderNode(QGraphicNode current);
    protected native boolean native_attachAudioNode(QAudioTrackNode child, QAudioTrackNode parent);
    protected native boolean native_detachAudioNode(QAudioTrackNode current);
    protected native long native_getPosition();
    protected native QRange native_getMediaTimeRange();
    protected native void native_target_release();
    //native ptr : don't modify it
    protected long mPtr = 0;
}
