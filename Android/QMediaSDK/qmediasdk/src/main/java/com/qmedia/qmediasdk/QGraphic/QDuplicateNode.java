package com.qmedia.qmediasdk.QGraphic;

import com.qmedia.qmediasdk.QEditor.QCombiner;

import java.util.UUID;

public class QDuplicateNode extends QGraphicNode {
    public QDuplicateNode(QGraphicNode fromNode, QCombiner combiner) {
        this(fromNode, combiner, UUID.randomUUID().toString());
    }
    //set id for index
    public QDuplicateNode(QGraphicNode fromNode, QCombiner combiner, String id) {
        super(combiner, id);
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
