package com.qmedia.qmediasdk.QTarget.Implements;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Matrix;
import android.opengl.EGL14;
import javax.microedition.khronos.egl.EGLContext;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.support.annotation.AttrRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;

import com.qmedia.qmediasdk.QCommon.QAspectLayout;
import com.qmedia.qmediasdk.QSource.QMediaDescribe;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;
import com.qmedia.qmediasdk.QTarget.QVideoRender;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by spring on 2017/6/19.
 */

public class QPlayerView extends QAspectLayout implements GLSurfaceView.Renderer , QVideoTarget {

    private static final String TAG = "QPlayerView";
    private static final boolean VERBOSE = false;

    protected GLSurfaceView mGLSurfaceView;
    private int mPreviewWidth;
    private int mPreviewHeight;

    //render control
//    protected volatile boolean mbDisplay = false;
//    private Object mDisplayLck = new Object();
//    private int mDisplayFps = 30;
//    private Timer mDisplayTimer;

    QVideoDescribe mVideodescribe;
    private QVideoRender mVideoRender;
    int[] viewPort = new int[4];
    float[] bkcolors = new float[4];
    private boolean isStarted = false;
    private boolean forceUpdate = false;

    public QPlayerView(@NonNull Context context) {
        super(context);

        mGLSurfaceView = new GLSurfaceView(context);
        init();
    }

    public QPlayerView(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);

        mGLSurfaceView = new GLSurfaceView(context, attrs);
        init();
    }

    public QPlayerView(@NonNull Context context, @Nullable AttributeSet attrs, @AttrRes int defStyleAttr) {
        super(context, attrs, defStyleAttr);

        mGLSurfaceView = new GLSurfaceView(context, attrs);
        init();
    }

    private void init() {
        Log.d(TAG, "XMBaseView init");

        //TODO: use customize setting
        //EXConfigChooser config = new EXConfigChooser();
        //mGLSurfaceView.setEGLConfigChooser(config);

        mGLSurfaceView.setEGLContextClientVersion(2);     // select GLES 2.0
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

    @Override
    public boolean startVideo() {
        isStarted = true;
        return true;
    }

    @Override
    public boolean stopVideo() {
        isStarted = false;
        return false;
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
    public void setRender(QVideoRender videoRender) {
        mVideoRender = videoRender;
    }

    @Override
    public QVideoRender getRender() {
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
                bmp = null;
                ByteArrayOutputStream bos = new ByteArrayOutputStream();
                dstbitmap.compress(Bitmap.CompressFormat.JPEG, 90, bos);
                dstbitmap.recycle();
                dstbitmap = null;
                captureBuffer = null;
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
    }

    //activity live
    public void onPause() {
//        mGLSurfaceView.onPause();
    }

    public void onDestroy() {

	    mGLSurfaceView.queueEvent(new Runnable() {
		    @Override public void run() {
			    // Tell the renderer that it's about to be paused so it can clean up.
			    mVideoRender.onVideoDestroy();
		    }
	    });
    }

    public void notifyRender(){
        mGLSurfaceView.requestRender();
    }


    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        GLES20.glGetFloatv(GLES20.GL_COLOR_CLEAR_VALUE, bkcolors,0);
        GLES20.glGetIntegerv(GLES20.GL_VIEWPORT, viewPort, 0);
        mVideoRender.onVideoCreate();
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mPreviewWidth = width;
        mPreviewHeight = height;

        GLES20.glGetIntegerv(GLES20.GL_VIEWPORT, viewPort, 0);

        //TODO: notify surface size changee
    }

    @Override
    public void onDrawFrame(GL10 gl) {

        GLES20.glClearColor(bkcolors[0],bkcolors[1],bkcolors[2],bkcolors[3]);
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        if (isStarted || forceUpdate){
            forceUpdate = false;
            mVideoRender.onVideoRender(-1);
        }
    }

    /**
     * Start display TimerTask
     */
//    protected void displayStart(){
//        synchronized (mDisplayLck) {
//            if (mDisplayTimer != null)
//                mDisplayTimer.cancel();
//
//            mDisplayTimer = new Timer();
//            mDisplayTimer.schedule(new TimerTask() {
//                @Override
//                public void run() {
//                    mPts += 1000/mVideodescribe.framerate; //pts plus a frame's duration
//                    mGLSurfaceView.requestRender();
//                }
//            }, 0, 1000 / mDisplayFps);
//
//            mbDisplay = true;
//        }
//    }
//    /**
//     * Stop display TimerTask
//     */
//    protected void displayStop(){
//        synchronized (mDisplayLck) {
//            if(mDisplayTimer != null) {
//                mDisplayTimer.cancel();
//                mDisplayTimer = null;
//            }
//
//            mbDisplay = false;
//        }
//    }

    public void queueEvent(Runnable run) {
        mGLSurfaceView.queueEvent(run);
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();

	    Log.d(TAG, "onDetachedFromWindow");

        removeView(mGLSurfaceView);
    }

    class EXConfigChooser implements GLSurfaceView.EGLConfigChooser {
        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            int attribs[] = {
                    EGL10.EGL_LEVEL, 0,
                    EGL10.EGL_RENDERABLE_TYPE, 4,  // EGL_OPENGL_ES2_BIT
                    EGL10.EGL_COLOR_BUFFER_TYPE, EGL10.EGL_RGB_BUFFER,
                    EGL10.EGL_RED_SIZE, 8,
                    EGL10.EGL_GREEN_SIZE, 8,
                    EGL10.EGL_BLUE_SIZE, 8,
                    EGL10.EGL_DEPTH_SIZE, 16,
                    EGL10.EGL_SAMPLE_BUFFERS, 1,
                    EGL10.EGL_SAMPLES, 4,  // This is for 4x MSAA.
                    EGL10.EGL_NONE
            };
            EGLConfig[] configs = new EGLConfig[1];
            int[] configCounts = new int[1];
            egl.eglChooseConfig(display, attribs, configs, 1, configCounts);

            if (configCounts[0] == 0) {
                // Failed! Error handling.
                return null;
            } else {
                return configs[0];
            }
        }
    }

    class ShareGLContextFactory implements GLSurfaceView.EGLContextFactory {
        private int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
        private EGLContext mShareContext = null;

        ShareGLContextFactory(EGLContext shareContext) {
            mShareContext = shareContext;
        }

        @Override
        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
            return egl.eglCreateContext(display, eglConfig, mShareContext, attrib_list);
        }

        @Override
        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            if (!egl.eglDestroyContext(display, context)) {
                Log.e("DefaultContextFactory", "display:" + display + " context: " + context);
            }
        }
    }
}
