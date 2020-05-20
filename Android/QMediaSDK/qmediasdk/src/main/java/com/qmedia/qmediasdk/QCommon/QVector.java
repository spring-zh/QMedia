package com.qmedia.qmediasdk.QCommon;

public class QVector {
    public QVector(float v0) {
        this.v0 = v0;
    }
    public QVector(float v0, float v1) {
        this.v0 = v0;
        this.v1 = v1;
    }
    public QVector(float v0, float v1, float v2) {
        this.v0 = v0;
        this.v1 = v1;
        this.v2 = v2;
    }
    public QVector(float v0, float v1, float v2, float v3) {
        this.v0 = v0;
        this.v1 = v1;
        this.v2 = v2;
        this.v3 = v3;
    }

    public float v0 = 0, v1 = 0, v2 = 0, v3 = 0;
}
