package com.qmedia.qmediasdk.QGraphic;

import com.qmedia.qmediasdk.QEditor.QCombiner;

public class QDuplicateNode extends QGraphicNode {
    public QDuplicateNode(QGraphicNode fromNode, QCombiner combiner) {
        super(combiner);
        this.fromNode = fromNode;
        mPtr = native_create(fromNode);
    }

    public QGraphicNode getFromNode() {
        return fromNode;
    }

    private QGraphicNode fromNode;

    //TODO: native
    protected native long native_create(QGraphicNode fromNode);
}
