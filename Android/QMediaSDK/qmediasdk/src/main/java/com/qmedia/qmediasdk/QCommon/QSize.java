package com.qmedia.qmediasdk.QCommon;

public class QSize {
    public QSize() {
        this.width = 0;
        this.height = 0;
    }

    public QSize(float width ,float height) {
        this.width = width;
        this.height = height;
    }

    boolean isZero() {
        return (width == 0 && height == 0);
    }

    public float width;
    public float height;
}
