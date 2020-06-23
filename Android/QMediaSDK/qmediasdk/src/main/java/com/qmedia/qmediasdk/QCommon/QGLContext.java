package com.qmedia.qmediasdk.QCommon;

import android.opengl.EGLContext;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import com.qmedia.qmediasdk.QCommon.gles.EglCore;
import com.qmedia.qmediasdk.QCommon.gles.GlUtil;


public class QGLContext {
    public static QGLContext shared() {
        return qglContext;
    }

    public EGLContext eglContext() {
        return eglCore.context();
    }

    public void useAsCurrentContext() {
        eglCore.makeCurrent(eglSurface);
    }

    public void unUseCurrent() {
        eglCore.makeNothingCurrent();
    }

    public int createTextureOES() {

        int[] textures = new int[1];
        GLES20.glGenTextures(1, textures, 0);
        GlUtil.checkGlError("glGenTextures");

        int texId = textures[0];
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texId);
        GlUtil.checkGlError("glBindTexture " + texId);

        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER,
                GLES20.GL_NEAREST);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER,
                GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S,
                GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T,
                GLES20.GL_CLAMP_TO_EDGE);
        GlUtil.checkGlError("glTexParameter");

        return texId;
    }

    private QGLContext() {
        eglCore = new EglCore(null, EglCore.FLAG_RECORDABLE);
        eglSurface = eglCore.createOffscreenSurface(1,1);
    }
    static QGLContext qglContext = new QGLContext();
    private EglCore eglCore = null;
    private EGLSurface eglSurface = null;
}
