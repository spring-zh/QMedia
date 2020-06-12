package com.qmedia.qmediasdk.QGraphic;

import com.qmedia.qmediasdk.QCommon.QSize;
import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QEditor.QCombiner;

import java.util.UUID;

public class QImageNode extends QGraphicNode {
    public QImageNode(String filePath, boolean inAsset , QCombiner combiner) {
        this(filePath, inAsset, combiner, UUID.randomUUID().toString());
    }

    public QImageNode(String filePath, boolean inAsset , QCombiner combiner, String id) {
        super(combiner, id);
        mPtr = native_create(filePath, inAsset);
        name = filePath;
        this.inAsset = inAsset;
    }

    public QSize getSize() {
        return native_getSize();
    }

    public String getFilePath() {
        return name;
    }
    public boolean isInAsset() { return inAsset; }

    private boolean inAsset;

    protected native long native_create(String filePath, boolean inAsset);
    protected native QSize native_getSize();
}
