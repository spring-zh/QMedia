package com.qmedia.qmediasdk.QGraphic;

import com.qmedia.qmediasdk.QEditor.QCombiner;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;

public class QVideoTrackNode extends QGraphicNode {
    public QVideoTrackNode(QMediaTrack mediaTrack, QCombiner combiner) {
        super(combiner);
        this.mediaTrack = mediaTrack;
        mPtr = native_create(mediaTrack);
    }
    public QMediaTrack getMediaTrack() {
        return mediaTrack;
    }

    @Override
    public void release() {
        super.release();
        mediaTrack = null;
    }

    private QMediaTrack mediaTrack;

    //TODO: native
    protected native long native_create(QMediaTrack mediaTrack);
}
