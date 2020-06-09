package com.qmedia.qmediasdk.QEditor;

import com.qmedia.qmediasdk.QAudio.QAudioTrackNode;
import com.qmedia.qmediasdk.QGraphic.QDuplicateNode;
import com.qmedia.qmediasdk.QGraphic.QGraphicNode;
import com.qmedia.qmediasdk.QSource.QMediaExtractorSource;
import com.qmedia.qmediasdk.QSource.QMediaSource;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

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
//    public ArrayList<QMediaTrack> getMediaTracks() {
//        return mediaTracks;
//    }
    public Map<String, QMediaTrack> getMediaTracks() {
        return mediaTracks;
    }
    protected boolean addMediaTrackIndex(QMediaTrack track) {
        if (! mediaTracks.containsValue(track)) {
            mediaTracks.put(track.getId(), track);
            return true;
        }else
            return false;
    }
    protected boolean removeMediaTrackIndex(QMediaTrack track) {
        if (mediaTracks.containsValue(track)) {
            mediaTracks.remove(track);
            return true;
        }else
            return false;
    }

    //TODO: graphicNodes @QGraphicNode
    public HashMap<String, QGraphicNode> getGraphicNodes() {
        return graphicNodes;
    }
    public boolean addGraphicNodeIndex(QGraphicNode node) {
        if (node instanceof QDuplicateNode) {
            return addDuplicateNodeIndex(node);
        }

        if (! graphicNodes.containsValue(node) && node != null) {
            graphicNodes.put(node.getId(), node);
            return true;
        }else
            return false;
    }
    public boolean removeGraphicNodeIndex(QGraphicNode node) {
        if (node instanceof QDuplicateNode) {
            return removeDuplicateNodeIndex(node);
        }

        if (node != null && graphicNodes.containsValue(node)) {
            graphicNodes.remove(node);
            return true;
        }else
            return false;
    }

    //TODO: duplicateNodes @QGraphicNode
    public HashMap<String, QGraphicNode> getDuplicateNodes() {
        return duplicateNodes;
    }
    private boolean addDuplicateNodeIndex(QGraphicNode node) {
        if (! duplicateNodes.containsValue(node) && node != null) {
            duplicateNodes.put(node.getId(), node);
            return true;
        }else
            return false;
    }
    private boolean removeDuplicateNodeIndex(QGraphicNode node) {
        if (node != null && duplicateNodes.containsValue(node)) {
            duplicateNodes.remove(node);
            return true;
        }else
            return false;
    }

    //TODO: duplicateNodes @QGraphicNode
    public HashMap<String, QAudioTrackNode> getAudioNodes() {
        return audioNodes;
    }
    public boolean addAudioNodeIndex(QAudioTrackNode node) {
        if (! audioNodes.containsValue(node) && node != null) {
            audioNodes.put(node.getId(), node);
            return true;
        }else
            return false;
    }
    public boolean removeAudioNodeIndex(QAudioTrackNode node) {
        if (node != null && audioNodes.containsValue(node)) {
            audioNodes.remove(node);
            return true;
        }else
            return false;
    }

    public QMediaTrack createVideoTrack(String filePath, boolean inAsset) {
        QMediaSource mediaSource = new QMediaExtractorSource(filePath, true, true, inAsset);
        mediaSource.setVideoTarget(videoTarget);
        mediaSource.setAudioTarget(audioTarget);
        QMediaTrack mediaTrack = new QMediaTrack(mediaSource);
        if (mediaTrack.prepare()) {
            weakCombiner.get().addMediaTrack(mediaTrack);
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
            weakCombiner.get().addMediaTrack(mediaTrack);
            mediaTrack.generateAudioTrackNode(weakCombiner.get());
            return mediaTrack;
        }else {
            mediaTrack.release();
            return null;
        }
    }

    private WeakReference<QCombiner> weakCombiner;
    //MediaTrack list and GraphicNode list for serialize index
//    protected ArrayList<QMediaTrack> mediaTracks = new ArrayList<>();
//    protected ArrayList<QGraphicNode> graphicNodes = new ArrayList<>();

    protected HashMap<String/*id*/, QMediaTrack> mediaTracks = new HashMap<>();
    protected HashMap<String/*id*/, QGraphicNode> graphicNodes = new HashMap<>();
    protected HashMap<String/*id*/, QGraphicNode> duplicateNodes = new HashMap<>();
    protected HashMap<String/*id*/, QAudioTrackNode> audioNodes = new HashMap<>();

    QVideoTarget videoTarget = null;
    QAudioTarget audioTarget = null;

    //TODO: native

    //native ptr : don't modify it
    private long mVideoTargetPtr = 0;
    private long mAudioTargetPtr = 0;
}
