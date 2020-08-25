package com.qmedia.qmediasdk.QEditor;

import android.util.Log;

import com.qmedia.qmediasdk.QAudio.QAudioTrackNode;
import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QCommon.QSize;
import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QEffect.QEffect;
import com.qmedia.qmediasdk.QGraphic.QBlendFunc;
import com.qmedia.qmediasdk.QGraphic.QDuplicateNode;
import com.qmedia.qmediasdk.QGraphic.QGraphicNode;
import com.qmedia.qmediasdk.QGraphic.QImageNode;
import com.qmedia.qmediasdk.QGraphic.QLayer;
import com.qmedia.qmediasdk.QGraphic.QVideoTrackNode;
import com.qmedia.qmediasdk.QMediaSDK;
import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QSource.QMediaExtractorSource;
import com.qmedia.qmediasdk.QSource.QMediaSource;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;
import com.qmedia.qmediasdk.QTrack.QMultiMediaTrack;

import java.util.HashMap;
import java.util.List;

public class QCombiner extends QMediaFactory{
    private static final String TAG = "QCombiner";

    public class QDisplayLayer extends QLayer {
        QDisplayLayer() {
            super(QCombiner.this, "0");
        }
        @Override
        public void setRenderRange(QRange renderRange) { }
        @Override
        public void setVisible(boolean visible) { }
        @Override
        public void setRotation3d(QVector rotation3d) { }
        @Override
        public void setRotation(float rotation) { }
        @Override
        public void setScaleX(float scaleX) { }
        @Override
        public void setScaleY(float scaleY) { }
        @Override
        public void setScaleZ(float scaleZ) { }
        @Override
        public void setContentSize(QSize contentSize) { }
        @Override
        public void setPosition(QVector position) { }
        @Override
        public void setPositionZ(float positionZ) { }
        @Override
        public void setAnchorPoint(QVector anchorPoint) { }
        @Override
        public void setColor4(QVector color4) { }
        @Override
        public void setAlpha(float alpha) { }
        @Override
        public void setCrop(QVector crop) { }
        @Override
        public void setBlendFunc(QBlendFunc blendFunc) { }
    }

    protected QDisplayLayer rootNode = new QDisplayLayer();

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

    public QDisplayLayer getRootNode() {
        return rootNode;
    }

    //TODO: mediaTracks @QMediaTrack
//    public ArrayList<QMediaTrack> getMediaTracks() {
//        return mediaTracks;
//    }
    public boolean addMediaTrack(QMediaTrack track) {
        boolean bRet = super.addMediaTrackIndex(track);
        if (bRet) {
            native_addMediaTrack(track);
        }
        return bRet;
    }
    public boolean removeMediaTrack(QMediaTrack track) {
        boolean bRet = super.removeMediaTrackIndex(track);
        if (bRet) {
            native_removeMediaTrack(track);
        }
        return bRet;
    }

    public boolean attachRenderNode(QGraphicNode child, QGraphicNode parent) {
        return native_attachRenderNode(child,parent);
    }

    public boolean detachRenderNode(QGraphicNode current) {
        return native_detachRenderNode(current);
    }

    public boolean attachEffect(QGraphicNode node, QEffect effect) {
        return native_attachEffect(node, effect);
    }

    public boolean detachEffect(QGraphicNode node, QEffect effect) {
        return native_detachEffect(node, effect);
    }

    public boolean attachAudioNode(QAudioTrackNode child, QAudioTrackNode parent) {
        return native_attachAudioNode(child,parent);
    }
    public boolean detachAudioNode(QAudioTrackNode current) {
        return native_detachAudioNode(current);
    }

    public long getPosition() {
        return native_getPosition();
    }

    public void setValidTimeRange(QRange timeRange) {
        native_setValidTimeRange(timeRange);
    }
    public QRange getMediaTimeRange() {
        return native_getMediaTimeRange();
    }

    public void copyForm(QCombiner from) {
        setValidTimeRange(from.getMediaTimeRange());

        graphicNodes.clear();
        //copy rootNode
        rootNode.copyForm(from.rootNode);
        rootNode.setEnable3d(from.rootNode.isEnable3d());
        rootNode.setBkColor(from.rootNode.getBkColor());
        super.addGraphicNodeIndex(rootNode);

        //copy mediaTracks
        for (HashMap.Entry<String, QMediaTrack> entry : from.mediaTracks.entrySet()) {
            QMediaTrack fromTrack = entry.getValue();
            QMediaSource fromSource = fromTrack.getMediaSource();
            QMediaTrack mediaTrack = null;
            if (fromTrack instanceof QMultiMediaTrack) {
                List<String> fromFileList = ((QMultiMediaTrack)fromTrack).getFileList();
                mediaTrack = new QMultiMediaTrack(fromFileList, this, fromTrack.getId());
            }else {
                if (fromSource instanceof QMediaExtractorSource) {
                    QMediaExtractorSource fromExtractorSource = (QMediaExtractorSource)fromSource;
                    QMediaSource mediaSource = new QMediaExtractorSource(fromExtractorSource.getFileName(),
                            fromExtractorSource.enableVideo(), fromExtractorSource.enableAudio(), fromExtractorSource.readInAsset());
                    mediaSource.setVideoTarget(videoTarget);
                    mediaSource.setAudioTarget(audioTarget);
                    mediaTrack = new QMediaTrack(mediaSource, fromTrack.getId());
                }
            }

            if (mediaTrack.prepare()) {
                mediaTrack.setDisplayRange(fromTrack.getDisplayRange());
                mediaTrack.setSourceRange(fromTrack.getSourceRange());
                mediaTrack.setTimeScale(fromTrack.getTimeScale());

                this.addMediaTrack(mediaTrack);
                if (fromTrack.getGraphic() != null) {
                    mediaTrack.generateVideoTrackNode(this, fromTrack.getGraphic().getId());
                    mediaTrack.getGraphic().copyForm(fromTrack.getGraphic());
                }
                if (fromTrack.getAudio() != null) {
                    mediaTrack.generateAudioTrackNode(this);
                    mediaTrack.getAudio().setPitch(fromTrack.getAudio().getPitch());
                    mediaTrack.getAudio().setVolume(fromTrack.getAudio().getVolume());
                }
            }
        }

        //copy graphic nodes;
        for (HashMap.Entry<String, QGraphicNode> entry: from.graphicNodes.entrySet()) {
            QGraphicNode newNode;
            QGraphicNode fromNode = entry.getValue();
            if (fromNode instanceof QDisplayLayer) {
                continue;
            }else if (fromNode instanceof QVideoTrackNode) {
                //TODO: set QVideoTrackNode
//                int index = from.mediaTracks.indexOf(((QVideoTrackNode) fromNode).getMediaTrack());
//                QMediaTrack mediaTrack = mediaTracks.get(index);
//                newNode = mediaTrack.getGraphic();
                continue;
            }else if (fromNode instanceof QLayer) {
                QLayer fromLayer = (QLayer)fromNode;
                QLayer layer = new QLayer(fromLayer.getLayerSize(),"", this, fromNode.getId());
                layer.setBkColor(fromLayer.getBkColor());
                newNode = layer;
            }else if (fromNode instanceof QImageNode) {
                QImageNode fromImage = (QImageNode)fromNode;
                QImageNode imageNode = new QImageNode(fromImage.getFilePath(),fromImage.isInAsset() , this, fromImage.getId());
                newNode = imageNode;
            }else {
                newNode = new QGraphicNode("", this, fromNode.getId());
            }

            newNode.copyForm(fromNode);
        }

        //copy duplicate nodes;
        for (HashMap.Entry<String, QGraphicNode> entry: from.duplicateNodes.entrySet()) {
            QDuplicateNode fromNode = (QDuplicateNode)entry.getValue();
            QGraphicNode findNode = graphicNodes.get(fromNode.getFromNode().getId());

            QGraphicNode newNode = new QDuplicateNode(findNode, this, fromNode.getId());
            newNode.copyForm(fromNode);
        }

        //copy render trees
        copyRenderTrees(from.rootNode, rootNode);
    }

    private void copyRenderTrees(QGraphicNode fromNode, QGraphicNode currentNode) {
        for (QGraphicNode graphicNode : fromNode.getChildrens()) {
            QGraphicNode childNode = graphicNodes.get(graphicNode.getId());
            if (childNode == null)
                childNode = duplicateNodes.get(graphicNode.getId());

            if (childNode != null) {
                currentNode.addChildNode(childNode);
                copyRenderTrees(graphicNode, childNode);
            }
        }
    }

    public void release() {
        //release all graphic node
        for (HashMap.Entry<String, QGraphicNode> entry: duplicateNodes.entrySet()) {
            entry.getValue().release();
        }
        for (HashMap.Entry<String, QGraphicNode> entry: graphicNodes.entrySet()) {
            entry.getValue().release();
        }
        duplicateNodes.clear();
        graphicNodes.clear();
        //release all audio node
        for (HashMap.Entry<String, QAudioTrackNode> entry: audioNodes.entrySet()) {
            entry.getValue().release();
        }
        audioNodes.clear();
        //release all media track
        for (HashMap.Entry<String, QMediaTrack> entry: mediaTracks.entrySet()) {
            entry.getValue().release();
        }
        mediaTracks.clear();
        native_target_release();
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
    protected native boolean native_attachEffect(QGraphicNode node, QEffect effect);
    protected native boolean native_detachEffect(QGraphicNode node, QEffect effect);
    protected native boolean native_attachAudioNode(QAudioTrackNode child, QAudioTrackNode parent);
    protected native boolean native_detachAudioNode(QAudioTrackNode current);
    protected native long native_getPosition();
    protected native void native_setValidTimeRange(QRange timeRange);
    protected native QRange native_getMediaTimeRange();
    private native void native_target_release();
    //native ptr : don't modify it
    protected long mPtr = 0;
}
