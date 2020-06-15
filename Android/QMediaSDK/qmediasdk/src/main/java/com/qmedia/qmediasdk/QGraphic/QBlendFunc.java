package com.qmedia.qmediasdk.QGraphic;

import android.opengl.GLES20;

public class QBlendFunc {
    // for blend
    public static final int Blend_ZERO = GLES20.GL_ZERO;
    public static final int Blend_ONE = GLES20.GL_ONE;
    public static final int Blend_SRC_COLOR = GLES20.GL_SRC_COLOR;
    public static final int Blend_ONE_MINUS_SRC_COLOR = GLES20.GL_ONE_MINUS_SRC_COLOR;
    public static final int Blend_SRC_ALPHA = GLES20.GL_SRC_ALPHA;
    public static final int Blend_ONE_MINUS_SRC_ALPHA = GLES20.GL_ONE_MINUS_SRC_ALPHA;
    public static final int Blend_DST_ALPHA = GLES20.GL_DST_ALPHA;
    public static final int Blend_ONE_MINUS_DST_ALPHA = GLES20.GL_ONE_MINUS_DST_ALPHA;
    public static final int Blend_DST_COLOR = GLES20.GL_DST_COLOR;
    public static final int Blend_ONE_MINUS_DST_COLOR = GLES20.GL_ONE_MINUS_DST_COLOR;
    public static final int Blend_SRC_ALPHA_SATURATE = GLES20.GL_SRC_ALPHA_SATURATE;

    static public QBlendFunc DISABLE = new QBlendFunc();
    static public QBlendFunc ALPHA_PREMULTIPLIED = new QBlendFunc(Blend_ONE, Blend_ONE_MINUS_SRC_ALPHA);
    static public QBlendFunc ALPHA_NON_PREMULTIPLIED = new QBlendFunc(Blend_SRC_ALPHA, Blend_ONE_MINUS_SRC_ALPHA);

    public QBlendFunc() {
        this.src = Blend_ONE;
        this.dst = Blend_ZERO;
    }

    public QBlendFunc(int src, int dst) {
        this.src = src;
        this.dst = dst;
    }
    public int src;
    public int dst;

}
