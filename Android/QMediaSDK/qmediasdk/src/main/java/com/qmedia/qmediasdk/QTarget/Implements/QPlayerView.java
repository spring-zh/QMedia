package com.qmedia.qmediasdk.QTarget.Implements;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.opengl.EGL14;

import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.GLES20;
import android.os.Build;
import android.support.annotation.AttrRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;

import com.qmedia.qmediasdk.QCommon.GLSurfaceView14;
import com.qmedia.qmediasdk.QCommon.QAspectLayout;
import com.qmedia.qmediasdk.QCommon.QGLContext;
import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QCommon.gles.FullFrameRect;
import com.qmedia.qmediasdk.QCommon.gles.GlUtil;
import com.qmedia.qmediasdk.QCommon.gles.Texture2dProgram;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;
import com.qmedia.qmediasdk.QTarget.QVideoRender;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by spring on 2017/6/19.
 */

public class QPlayerView extends QAspectLayout implements GLSurfaceView14.Renderer , QVideoTarget {

    private static final String TAG = "QPlayerView";
    private static final boolean VERBOSE = false;

    protected GLSurfaceView14 mGLSurfaceView;
    private int mPreviewWidth;
    private int mPreviewHeight;

    public enum DisplayMode {
        Stretch,
        Adaptive,
        Clip
    }

    private DisplayMode displayMode = DisplayMode.Stretch;

    QVideoDescribe mVideodescribe;
    private QVideoRender mVideoRender;
    int[] viewPort = new int[4];
    float[] bkcolors = new float[4];
    private boolean isStarted = false;
    private boolean forceUpdate = false;

    public QPlayerView(@NonNull Context context) {
        super(context);

        mGLSurfaceView = new GLSurfaceView14(context);
        init();
    }

    public QPlayerView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);

        mGLSurfaceView = new GLSurfaceView14(context, attrs);
        init();
    }

    public QPlayerView(@NonNull Context context, @Nullable AttributeSet attrs, @AttrRes int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        mGLSurfaceView = new GLSurfaceView14(context, attrs);
        init();
    }

    private void init() {
        Log.d(TAG, "QPlayerView init");

        //TODO: use customize setting
        mGLSurfaceView.setEGLContextClientVersion(2);     // select GLES 2.0
        mGLSurfaceView.setEGLConfigChooser(new EXConfigChooser());
        mGLSurfaceView.setEGLContextFactory(new ShareGLContextFactory(QGLContext.shared().eglContext()));
        mGLSurfaceView.setRenderer(this);
//        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        mGLSurfaceView.setPreserveEGLContextOnPause(true);

        addView(mGLSurfaceView);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        forceUpdate = true;
//        notifyRender();
    }

    public void setDisplayMode(DisplayMode mode) {
        displayMode = mode;
    }

    QVector calculateViewPort(DisplayMode mode, int srcW, int srcH, int dstW, int dstH) {
        QVector dstRegion = new QVector(0,0,0,0);
//        int []ret = {0,0,0,0};
        switch (mode) {
            case Stretch:
                dstRegion.v2 = dstW;
                dstRegion.v3 = dstH;
                break;
            case Adaptive:
            {
                float dstRatio = (float) dstW / dstH;
                float srcRatio = (float) srcW / srcH;
                if (srcRatio > dstRatio) {
                    dstRegion.v0 = 0;
                    dstRegion.v2 = dstW;
                    int newH = (int) (dstW / srcRatio);
                    dstRegion.v1 = (dstH - newH)/2;
                    dstRegion.v3 = (dstH + newH)/2;
                } else {
                    dstRegion.v1 = 0;
                    dstRegion.v3 = dstH;
                    int newW = (int) (dstH * srcRatio);
                    dstRegion.v0 = (dstW - newW) / 2;
                    dstRegion.v2 = (dstW + newW) / 2;
                }
            }
                break;
            case Clip:

                break;

        }
        return dstRegion;
    }

    @Override
    public boolean startVideo() {
//        mGLSurfaceView.queueEvent(new Runnable() {
//            @Override
//            public void run() {
//                mVideoRender.onVideoCreate();
//            }
//        });
        synchronized (this){
            isStarted = true;
        }
        return true;
    }

    @Override
    public boolean stopVideo() {
        synchronized (this) {
            isStarted = false;
        }
        mGLSurfaceView.queueEvent(new Runnable() {
            @Override
            public void run() {
                mVideoRender.onVideoDestroy();
                releaseTextureTarget();
            }
        });
        return true;
    }

    @Override
    public void pauseVideo(boolean isPause) {
        isStarted = !isPause;
    }

    @Override
    public void flushVideo() {
        forceUpdate = true;
//        notifyRender();
    }

    @Override
    public boolean initVideo(QVideoDescribe describe) {
        mVideodescribe = describe;
        return false;
    }

    @Override
    public int getTargetWidth() {
        return  mVideodescribe.width;
    }

    @Override
    public int getTargetHeight() {
        return mVideodescribe.height;
    }

    @Override
    public float getFrameRate() {
        return mVideodescribe.framerate;
    }

    @Override
    public Object getTargetContext() {
        return null;
    }

    @Override
    public void setVideoRender(QVideoRender videoRender) {
        mVideoRender = videoRender;
    }

    @Override
    public QVideoRender getVideoRender() {
        return mVideoRender;
    }

    public boolean captureViewPicture(final String filename){

        mGLSurfaceView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ByteBuffer captureBuffer = ByteBuffer.allocateDirect(mPreviewWidth * mPreviewHeight * 4);
                Bitmap bmp = Bitmap.createBitmap(mPreviewWidth, mPreviewHeight, Bitmap.Config.ARGB_8888);
                captureBuffer.rewind();
                GLES20.glReadPixels(0, 0, mPreviewWidth, mPreviewHeight, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE,
                        captureBuffer);
                captureBuffer.rewind();
                bmp.copyPixelsFromBuffer(captureBuffer);
                float[] floats = new float[] { 1f, 0f, 0f, 0f, -1f, 0f, 0f, 0f, 1f };
                Matrix matrix = new Matrix();
                matrix.setValues(floats);
                Bitmap dstbitmap = Bitmap.createBitmap(bmp,0, 0, mPreviewWidth, mPreviewHeight, matrix, true);
                bmp.recycle();
                ByteArrayOutputStream bos = new ByteArrayOutputStream();
                dstbitmap.compress(Bitmap.CompressFormat.JPEG, 90, bos);
                dstbitmap.recycle();
                try {
                    FileOutputStream fos = new FileOutputStream(new File(filename));
                    bos.writeTo(fos);
                    bos.close();
                    fos.close();
                } catch (FileNotFoundException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        return true;
    }

    public void onResume() {
        mGLSurfaceView.onResume();
        mGLSurfaceView.queueEvent(new Runnable() {
            @Override public void run() {
                // Tell the renderer that it's about to be paused so it can clean up.
                //TODO: resume gl resource
//                mVideoRender.onVideoCreate();
            }
        });
        forceUpdate = true;
    }

    //activity live
    public void onPause() {
        mGLSurfaceView.onPause();
    }

    public void onDestroy() {

//	    mGLSurfaceView.queueEvent(new Runnable() {
//		    @Override public void run() {
//			    // Tell the renderer that it's about to be paused so it can clean up.
//			    mVideoRender.onVideoDestroy();
//		    }
//	    });
    }

    boolean bTextureCreate = false;
    private FullFrameRect mFullScreen;
    private int []targetTexure = new int[1];
    private int []targetFrameBuffer = new int[1];

    void createTextureTarget(int width, int height){
        if (bTextureCreate) {
            return;
        }
        mFullScreen = new FullFrameRect(
                new Texture2dProgram(Texture2dProgram.ProgramType.TEXTURE_2D));
        targetTexure[0] = mFullScreen.createTextureObject();
        GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, /*level*/ 0, GLES20.GL_RGBA,
                width, height, /*border*/ 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, null);
        GlUtil.checkGlError("loadImageTexture");
        GLES20.glGenFramebuffers(1, targetFrameBuffer,0);

        bTextureCreate = true;
    }

    void releaseTextureTarget() {
        if (!bTextureCreate)
            return;
        GLES20.glDeleteTextures(1,targetTexure, 0);
        GLES20.glDeleteFramebuffers(1, targetFrameBuffer, 0);
        if (mFullScreen != null) {
            mFullScreen.release(false);
            mFullScreen = null;
        }
        bTextureCreate = false;
    }

    void setRenderTexture(int texId) {
        if (texId > 0) {
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, targetFrameBuffer[0]);
            GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0,
                    GLES20.GL_TEXTURE_2D, texId, 0);
        }else {
            GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        }

        int status = GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER);
        if (status != GLES20.GL_FRAMEBUFFER_COMPLETE)
        {
            GlUtil.checkGlError("glCheckFramebufferStatus: " +  status);
        }
    }


    @Override
    public void onSurfaceCreated(EGLConfig config) {
        GLES20.glGetFloatv(GLES20.GL_COLOR_CLEAR_VALUE, bkcolors,0);
        GLES20.glGetIntegerv(GLES20.GL_VIEWPORT, viewPort, 0);
        mVideoRender.onVideoCreate();
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        mPreviewWidth = width;
        mPreviewHeight = height;

        GLES20.glGetIntegerv(GLES20.GL_VIEWPORT, viewPort, 0);

        //TODO: notify surface size changee
    }

    @Override
    public boolean onDrawFrame() {
        createTextureTarget(mVideodescribe.width, mVideodescribe.height);
        synchronized (this) {
            if (isStarted || forceUpdate) {
                forceUpdate = false;
                setRenderTexture(targetTexure[0]);
                GLES20.glViewport(0,0, mVideodescribe.width, mVideodescribe.height);
                mVideoRender.onVideoRender(-1);
                setRenderTexture(0);

                GLES20.glDisable(GLES20.GL_DEPTH_TEST);

                QVector viewVec = calculateViewPort(displayMode, mVideodescribe.width, mVideodescribe.height, mPreviewWidth, mPreviewHeight);
                GLES20.glViewport((int)viewVec.v0, (int)viewVec.v1, (int)(viewVec.v2 - viewVec.v0), (int)(viewVec.v3 - viewVec.v1));
                GLES20.glClearColor(bkcolors[0],bkcolors[1],bkcolors[2],bkcolors[3]);
                GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);

                mFullScreen.drawFrame(targetTexure[0], null);

                return true;
            }
        }
        return false;
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();

	    Log.d(TAG, "onDetachedFromWindow");

        removeView(mGLSurfaceView);
    }

    private static final int EGL_RECORDABLE_ANDROID = 0x3142;
    class EXConfigChooser implements GLSurfaceView14.EGLConfigChooser {
        @Override
        public EGLConfig chooseConfig(EGLDisplay display) {
            int attribs[] = {
                EGL14.EGL_RED_SIZE, 8,
                        EGL14.EGL_GREEN_SIZE, 8,
                        EGL14.EGL_BLUE_SIZE, 8,
                        EGL14.EGL_DEPTH_SIZE, 16,
                        EGL14.EGL_SAMPLE_BUFFERS, 1,
//                    EGL14.EGL_SINGLE_BUFFER, 1,
                        EGL14.EGL_SURFACE_TYPE, /*EGL14.EGL_PBUFFER_BIT |*/ EGL14.EGL_WINDOW_BIT,
                        EGL_RECORDABLE_ANDROID, 1,
                        EGL14.EGL_SAMPLES, 4,  // This is for 4x MSAA.
                        EGL14.EGL_NONE
            };
            if (Build.CPU_ABI.equals("x86")) {
                int new_attribs[] = {
                        EGL14.EGL_RED_SIZE, 8,
                        EGL14.EGL_GREEN_SIZE, 8,
                        EGL14.EGL_BLUE_SIZE, 8,
                        EGL14.EGL_DEPTH_SIZE, 16,
                        EGL14.EGL_NONE
                };
                attribs = new_attribs;
            }

            EGLConfig[] configs = new EGLConfig[1];
            int[] configCounts = new int[1];
            EGL14.eglChooseConfig(display, attribs, 0, configs, 0,1, configCounts, 0);

            if (configCounts[0] == 0) {
                // Failed! Error handling.
                return null;
            } else {
                return configs[0];
            }
        }
    }

    class ShareGLContextFactory implements GLSurfaceView14.EGLContextFactory {
        private int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
        private EGLContext mShareContext = null;

        ShareGLContextFactory(EGLContext shareContext) {
            mShareContext = shareContext;
        }

        @Override
        public EGLContext createContext(EGLDisplay display, EGLConfig eglConfig) {
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL14.EGL_NONE };
            return EGL14.eglCreateContext(display, eglConfig, mShareContext, attrib_list, 0);
        }

        @Override
        public void destroyContext(EGLDisplay display, EGLContext context) {
            if (!EGL14.eglDestroyContext(display, context)) {
                Log.e("DefaultContextFactory", "display:" + display + " context: " + context);
            }
        }
    }
}
