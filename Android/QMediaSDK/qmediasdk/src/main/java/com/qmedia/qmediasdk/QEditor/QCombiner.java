package com.qmedia.qmediasdk.QEditor;

import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QGraphic.QGraphicNode;
import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;

import java.util.ArrayList;

public class QCombiner {

    public void setVideoTarget(QVideoTarget videoTarget) {
        mediaFactory.setVideoTarget(videoTarget, this);
    }

    public void setAudioTarget(QAudioTarget audioTarget) {
        mediaFactory.setAudioTarget(audioTarget, this);
    }

    public void setVideoConfig(QVideoDescribe describe) {
        native_setVideoConfig(describe);
    }
    public void setAudioConfig(QAudioDescribe describe) {
        native_setAudioConfig(describe);
    }

    //TODO: mediaTracks @QMediaTrack
    public ArrayList<QMediaTrack> getMediaTracks() {
        return mediaTracks;
    }
    public void addMediaTrack(QMediaTrack track) {
        if (! mediaTracks.contains(track)) {
            native_addMediaTrack(track);
            mediaTracks.add(track);
        }
    }
    public void removeMediaTrack(QMediaTrack track) {
        if (mediaTracks.contains(track)) {
            native_removeMediaTrack(track);
            mediaTracks.remove(track);
        }
    }

    //TODO: graphicNodes @QGraphicNode
    public ArrayList<QGraphicNode> getGraphicNodes() {
        return graphicNodes;
    }
    public void addGraphicNode(QGraphicNode node) {
        if (! graphicNodes.contains(node)) {
            graphicNodes.add(node);
        }
    }
    public void removeGraphicNode(QGraphicNode node) {
        if (graphicNodes.contains(node)) {
            graphicNodes.remove(node);
        }
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

    protected ArrayList<QMediaTrack> mediaTracks = new ArrayList<>();
    protected ArrayList<QGraphicNode> graphicNodes = new ArrayList<>();

    protected QMediaFactory mediaFactory = new QMediaFactory();

    //TODO: native
    protected native void native_setVideoConfig(QVideoDescribe describe);
    protected native void native_setAudioConfig(QAudioDescribe describe);
    protected native void native_setVideoTarget(QVideoTarget videoTarget);
    protected native void native_setAudioTarget(QAudioTarget audioTarget);
    protected native void native_addMediaTrack(QMediaTrack track);
    protected native void native_removeMediaTrack(QMediaTrack track);
    protected native boolean native_attachRenderNode(QGraphicNode child, QGraphicNode parent);
    protected native boolean native_detachRenderNode(QGraphicNode current);
    protected native long native_getPosition();
    protected native QRange native_getMediaTimeRange();
    //native ptr : don't modify it
    protected long mPtr = 0;
}
