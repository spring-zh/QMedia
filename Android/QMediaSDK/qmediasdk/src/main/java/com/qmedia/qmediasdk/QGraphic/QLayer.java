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

    protected QLayer(QCombiner combiner, String id) {
        super(combiner, id);
    }

    public QSize getLayerSize() {
        return layerSize;
    }

    public QVector getBkColor() {
        return native_getBkColor();
    }

    public void setBkColor(QVector bkColor) {
        native_setBkColor(bkColor);
    }

    public boolean isEnable3d() {
        return native_isEnable3d();
    }

    public void setEnable3d(boolean enable3d) {
        native_setEnable3d(enable3d);
    }

    public int getFlipMode() {
        return native_getFlipMode();
    }

    public void setFlipMode(int flipMode) {
        native_setFlipMode(flipMode);
    }

    private QSize layerSize;
    //TODO: native
    protected native long native_create(QSize size);
    protected native QVector native_getBkColor();
    protected native void native_setBkColor(QVector bkColor);
    protected native boolean native_isEnable3d();
    protected native void native_setEnable3d(boolean enable3d);
    protected native int native_getFlipMode();
    protected native void native_setFlipMode(int flipMode);
}
