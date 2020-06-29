package com.qmedia.qmediasdk.QGraphic;

import com.qmedia.qmediasdk.QCommon.QSize;
import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QEditor.QCombiner;
import java.util.UUID;

public class QLayer extends QGraphicNode {
    //size v2
    public QLayer(QSize size, String name, QCombiner combiner) {
        this(size, name, combiner, UUID.randomUUID().toString());
    }

    public QLayer(QSize size, String name, QCombiner combiner, String id) {
        super(combiner, id);
        mPtr = native_create(size);
        setName(name);
        layerSize = size;
    }

    public QSize getLayerSize() {
        return layerSize;
    }

    public QVector getBkColor() {
        return native_getBkColor();
    }

    public void setBkColor(QVector bkColor) {
        this.bkColor = bkColor;
        native_setBkColor(bkColor);
    }

    private QSize layerSize;
    private QVector bkColor;
    //TODO: native
    protected native long native_create(QSize size);
    protected native QVector native_getBkColor();
    protected native void native_setBkColor(QVector bkColor);
}
