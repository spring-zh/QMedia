package com.qmedia.qmediasdk.QEditor;

import com.qmedia.qmediasdk.QGraphic.QDuplicateNode;
import com.qmedia.qmediasdk.QGraphic.QGraphicNode;
import com.qmedia.qmediasdk.QSource.QMediaExtractorSource;
import com.qmedia.qmediasdk.QSource.QMediaSource;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

public class QMediaFactory {

    protected void setCombiner(QCombiner selfCombiner) {
        weakCombiner = new WeakReference<>(selfCombiner);
    }

    public QVideoTarget getVideoTarget() {
        return videoTarget;
    }

    public QAudioTarget getAudioTarget() {
        return audioTarget;
    }

    //TODO: mediaTracks @QMediaTrack
    public ArrayList<QMediaTrack> getMediaTracks() {
        return mediaTracks;
    }
    protected boolean addMediaTrack(QMediaTrack track) {
        if (! mediaTracks.contains(track)) {
            return mediaTracks.add(track);
        }else
            return false;
    }
    protected boolean removeMediaTrack(QMediaTrack track) {
        if (mediaTracks.contains(track)) {
            return mediaTracks.remove(track);
        }else
            return false;
    }

    //TODO: graphicNodes @QGraphicNode
    public ArrayList<QGraphicNode> getGraphicNodes() {
        return graphicNodes;
    }
    public boolean addGraphicNode(QGraphicNode node) {
        if (! graphicNodes.contains(node) && node != null) {
            int index = 0;
            if (node instanceof QDuplicateNode) {
                for (QGraphicNode graphicNode : graphicNodes){
                    if (graphicNode instanceof QDuplicateNode) {
                        graphicNodes.add(index, node);
                        return true;
                    }
                    index ++;
                }
            }
            graphicNodes.add(node);
            return true;
        }else
            return false;
    }
    public boolean removeGraphicNode(QGraphicNode node) {
        if (node != null && graphicNodes.contains(node)) {
            return graphicNodes.remove(node);
        }else
            return false;
    }


    public QMediaTrack createVideoTrack(String filePath, boolean inAsset) {
        QMediaSource mediaSource = new QMediaExtractorSource(filePath, true, true, inAsset);
        mediaSource.setVideoTarget(videoTarget);
        mediaSource.setAudioTarget(audioTarget);
        QMediaTrack mediaTrack = new QMediaTrack(mediaSource);
        if (mediaTrack.prepare()) {
            addMediaTrack(mediaTrack);
            mediaTrack.generateVideoTrackNode(weakCombiner.get());
            mediaTrack.generateAudioTrackNode(weakCombiner.get());
            return mediaTrack;
        }else {
            mediaTrack.release();
            return null;
        }
    }

    public QMediaTrack createAudioTrack(String filePath, boolean inAsset) {
        QMediaSource mediaSource = new QMediaExtractorSource(filePath, false, true, inAsset);
        mediaSource.setVideoTarget(videoTarget);
        mediaSource.setAudioTarget(audioTarget);
        QMediaTrack mediaTrack = new QMediaTrack(mediaSource);
        if (mediaTrack.prepare()) {
            addMediaTrack(mediaTrack);
            mediaTrack.generateAudioTrackNode(weakCombiner.get());
            return mediaTrack;
        }else {
            mediaTrack.release();
            return null;
        }
    }

    private WeakReference<QCombiner> weakCombiner;
    //MediaTrack list and GraphicNode list for serialize index
    protected ArrayList<QMediaTrack> mediaTracks = new ArrayList<>();
    protected ArrayList<QGraphicNode> graphicNodes = new ArrayList<>();

    QVideoTarget videoTarget = null;
    QAudioTarget audioTarget = null;

    //TODO: native

    //native ptr : don't modify it
    private long mVideoTargetPtr = 0;
    private long mAudioTargetPtr = 0;
}
