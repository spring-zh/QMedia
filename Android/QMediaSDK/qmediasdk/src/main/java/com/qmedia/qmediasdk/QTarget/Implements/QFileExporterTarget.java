package com.qmedia.qmediasdk.QTarget.Implements;


import android.media.MediaCodec;
import android.media.MediaFormat;
import android.opengl.EGLContext;
import android.opengl.GLES20;
import android.util.Log;

import com.qmedia.qmediasdk.QCommon.QGLContext;
import com.qmedia.qmediasdk.QCommon.gles.EglCore;
import com.qmedia.qmediasdk.QCommon.gles.FullFrameRect;
import com.qmedia.qmediasdk.QCommon.gles.GlUtil;
import com.qmedia.qmediasdk.QCommon.gles.WindowSurface;
import com.qmedia.qmediasdk.QCommon.media.HardwareEncoder;
import com.qmedia.qmediasdk.QCommon.media.MuxerCore;
import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;
import com.qmedia.qmediasdk.QTarget.QAudioRender;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoRender;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

/**
 * Created by spring on 2017/5/8.
 */

public class QFileExporterTarget implements QVideoTarget, QAudioTarget {

    private static final String TAG = "QFileExporterTarget";

    VideoThread mVideoThread;
    AudioThread mAudioThread;

    MuxerCore mMuxerCore;
    private String mOutputFileName = "";

    private boolean bInit = false;

    public QFileExporterTarget(String fileName) {
        Log.d(TAG, "QFileExporterTarget new." + this.toString());
        mOutputFileName = fileName;
    }

    private boolean mbEnableAudio = true;
    private boolean mbKeyFrameMode = false;

    private QVideoDescribe mVideoConfig = null;
    private QAudioDescribe mAudioConfig = null;

    WeakReference<QAudioRender> mAudioRender = null;
    WeakReference<QVideoRender> mVideoRender = null;

    public void setKeyFrameMode(boolean isAllKeyFrame)
    {
        mbKeyFrameMode = isAllKeyFrame;
    }

    public void setEnableAudio(boolean enable){
        mbEnableAudio = enable;
    }

    @Override
    public boolean startAudio() {
        Log.i(TAG, "startAudio");
        if ( mAudioConfig == null || !mbEnableAudio)
            return false;

        if (mAudioThread != null)
            return true;
        try {
            if (mMuxerCore == null)
                mMuxerCore = new MuxerCore(mOutputFileName,true);

            mAudioThread = new AudioThread(mAudioConfig);
            mAudioThread.start();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    @Override
    public boolean stopAudio() {
        Log.i(TAG, "stopAudio");
//        if (mMuxerCore != null)
//            mMuxerCore.abortWait();

        if (mAudioThread != null) {
            mAudioThread.determine();
            mAudioThread = null;
        }
        if (mVideoThread == null && mAudioThread == null && mMuxerCore != null){
            mMuxerCore.abortWait();
            mMuxerCore.close();
            mMuxerCore = null;
        }
        return true;
    }

    @Override
    public void pauseAudio() {

    }

    @Override
    public void resumeAudio() {

    }

    @Override
    public void flushAudio() {

    }

    @Override
    public boolean initAudio(QAudioDescribe describe) {
        Log.i(TAG, "initAudio describe.samplerate:"+ describe.samplerate + " nchannel:" + describe.nchannel);
        mAudioConfig = describe;
        return true;
    }

    @Override
    public void unInitAudio() {
        Log.i(TAG, "unInitAudio");
    }

    @Override
    public int getSampleRate() {
        return mAudioConfig.samplerate;
    }

    @Override
    public int getChannels() {
        return mAudioConfig.nchannel;
    }

    @Override
    public int getFormat() {
        return mAudioConfig.rawFormat;
    }

    @Override
    public void setVolume(float volume) {

    }

    @Override
    public float getVolume() {
        return 0;
    }

    @Override
    public void setAudioDelay(int audioDelay) {

    }

    @Override
    public int getAudioDelay() {
        return 0;
    }

    @Override
    public void setAudioRender(QAudioRender audioRender) {
        mAudioRender = new WeakReference<>(audioRender);
    }

    @Override
    public QAudioRender getAudioRender() {
        return mAudioRender.get();
    }

    @Override
    public boolean startVideo() {
        Log.i(TAG, "startVideo");
        if (mVideoConfig == null)
            return false;
        if (mVideoThread != null)
            return true;
        try {
            if (mMuxerCore == null)
                mMuxerCore = new MuxerCore(mOutputFileName,mbEnableAudio && (mAudioConfig != null));

            mVideoThread = new VideoThread(mVideoConfig);
            mVideoThread.start();
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
        return false;
    }

    @Override
    public boolean stopVideo() {
        Log.i(TAG, "stopVideo");
//        if (mMuxerCore != null)
//            mMuxerCore.abortWait();

        if (mVideoThread != null) {
            mVideoThread.determine();
            mVideoThread = null;
        }
        if (mVideoThread == null && mAudioThread == null && mMuxerCore != null){
            mMuxerCore.abortWait();
            mMuxerCore.close();
            mMuxerCore = null;
        }
        return true;
    }

    @Override
    public void pauseVideo(boolean isPause) {

    }

    @Override
    public void flushVideo() {

    }

    @Override
    public boolean initVideo(QVideoDescribe describe) {
        Log.i(TAG, "initAudio describe.framerate:"+ describe.framerate + " width:" + describe.width + " height:" + describe.height);
        mVideoConfig = describe;
        return true;
    }

    @Override
    public int getTargetWidth() {
        return mVideoConfig.width;
    }

    @Override
    public int getTargetHeight() {
        return mVideoConfig.height;
    }

    @Override
    public float getFrameRate() {
        return mVideoConfig.framerate;
    }

    @Override
    public Object getTargetContext() {
        return null;
    }

    @Override
    public void setVideoRender(QVideoRender videoRender) {
        mVideoRender = new WeakReference<>(videoRender);
    }

    @Override
    public QVideoRender getVideoRender() {
        return mVideoRender.get();
    }

    class VideoThread extends Thread implements HardwareEncoder.Listener{

        private static final boolean VERBOSE = false;

        // ----- accessed exclusively by encoder thread -----
        private WindowSurface mInputWindowSurface;
        private EglCore mEglCore;
        private FullFrameRect mFullScreen;
        private int []targetTexure = new int[1];
        private int []targetFrameBuffer = new int[1];

        private HardwareEncoder mVideoEncoder;
        boolean mThreadRuning = false;

        private int mWidth;
        private int mHeight;

        long mTimeDuration = 0;
        long mLastWriteTime = 0;


        VideoThread(QVideoDescribe describe) throws IOException {
            setName("VideoEncodeThread");
            mVideoEncoder = new HardwareEncoder(this,false);
            mVideoEncoder.startVideoEncoder((int) describe.framerate,describe.width,describe.height,describe.bitrate,false);
//            mSharedContext = conf.mEglContext;
            mTimeDuration = (long) (1000*1000/ describe.framerate);

            mWidth = describe.width;
            mHeight = describe.height;
        }

        @Override
        public void start(){
            mThreadRuning = true;
            super.start();
        }

        public void determine(){
            mThreadRuning = false;
            try {
                join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            mVideoEncoder.release();
        }

        private void prepareEGL(EGLContext sharedContext) {

            mEglCore = new EglCore(sharedContext, EglCore.FLAG_RECORDABLE);
            mInputWindowSurface = new WindowSurface(mEglCore, mVideoEncoder.getInputSurface(), true);
            mInputWindowSurface.makeCurrent();
            Log.i(TAG, "GL_EXTENSIONS : " + GLES20.glGetString(GLES20.GL_EXTENSIONS));
        }
        private void releaseEGL() {

            if (mInputWindowSurface != null) {
                mInputWindowSurface.release();
                mInputWindowSurface = null;
            }
            if (mEglCore != null) {
                mEglCore.release();
                mEglCore = null;
            }
        }

        @Override
        public void run() {

            long timestampUs = 0;
            prepareEGL(QGLContext.shared().eglContext());
            GLES20.glViewport(0,0,mWidth,mHeight);
            GlUtil.checkGlError("after prepareEGL");

            getVideoRender().setDisplayMode(0,mWidth,mHeight);
            getVideoRender().onVideoCreate();

            while (mThreadRuning){
//                mVideoEncoder.requireKeyFrame();
                mVideoEncoder.drainEncoder(false);

                GlUtil.checkGlError("onVideoRender before");
                GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
                int status = GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER);
                if (status != GLES20.GL_FRAMEBUFFER_COMPLETE)
                {
                    GlUtil.checkGlError("glCheckFramebufferStatus: " +  status);
                }
                GLES20.glViewport(0,0,mWidth,mHeight);
                if (getVideoRender().onVideoRender(timestampUs / 1000))
                {
                    GlUtil.checkGlError("onVideoRender after");
                    mInputWindowSurface.setPresentationTime((timestampUs + 10)*1000);
                    mInputWindowSurface.swapBuffers();
//                    mVideoEncoder.requireKeyFrame();
                    timestampUs += mTimeDuration;
                }
                else //video stream end
                {
                    Log.i(TAG,"video stream not ready or not right.");
                    mVideoEncoder.drainEncoder(false);
                    break;
                }

            }

            mVideoEncoder.drainEncoder(true);

            getVideoRender().onVideoDestroy();
            releaseEGL();
        }

        @Override
        public void onFormatChange(MediaFormat newformat) {
            mMuxerCore.setTrackFormat(newformat,false);
        }
        @Override
        public void onInputAvaliable(ByteBuffer inData, int[] size, long[] timeUs, int remaining)
        {

        }
        @Override
        public void onGetData(ByteBuffer encodedData, MediaCodec.BufferInfo bufferInfo) {
            //Log.d(TAG, "GETAV onGetData VIDEO bufferInfo:" + bufferInfo.presentationTimeUs);

            mLastWriteTime = bufferInfo.presentationTimeUs;
            mMuxerCore.waitToWrite();
            mMuxerCore.writeSample(encodedData,bufferInfo,false);
        }

    }

    class AudioThread extends Thread implements HardwareEncoder.Listener{

        boolean mThreadRuning = false;
        int requestSize;

        HardwareEncoder mAudioEncoder;

        int mAudioTimestampUs ;
        int bytesPerSecond ;
        long mLastWriteTime = 0;

        AudioThread(QAudioDescribe describe) throws IOException {
            setName("AudioEncodeThread");
            mAudioEncoder = new HardwareEncoder(this,true);
            mAudioEncoder.startAudioEncoder(describe.samplerate,describe.nchannel, describe.bitrate);

            //set size bytes for each packet
            requestSize = 1024 * describe.nchannel * 2;

            mAudioTimestampUs = 0;
            bytesPerSecond = describe.samplerate * describe.nchannel * 2;
        }

        @Override
        public void start(){
            mThreadRuning = true;
            super.start();
        }

        public void determine(){
            mThreadRuning = false;
            try {
                join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }


        @Override
        public void run() {
            long begin = System.currentTimeMillis();
            while (mThreadRuning){

                mAudioEncoder.drainEncoder(false);

                mAudioTimestampUs += 1000000L * requestSize / bytesPerSecond;

                ByteBuffer byteBuffer = ByteBuffer.allocate(requestSize);
                if(getAudioRender().onAudioRender(byteBuffer.array(), requestSize, mAudioTimestampUs/1000))
                {
                    //Log.e(TAG,"GETAV_AUDIO readLen " + readLen[0] + " pts " + timeUs[0]);
                    byteBuffer.rewind();
                    boolean bufput;
                    do {
                        bufput = mAudioEncoder.encodeData(byteBuffer, requestSize , mAudioTimestampUs);
                    }while (!bufput && mThreadRuning);

                }
                else {
                    mAudioEncoder.drainEncoder(false);
                    break;
                }

            }

            mAudioEncoder.drainEncoder(true);
            Log.d(TAG, "MediaStat AUDIO encoder cost=" + (System.currentTimeMillis() - begin));
        }

        @Override
        public void onFormatChange(MediaFormat newformat) {
            mMuxerCore.setTrackFormat(newformat,true);
        }
        @Override
        public void onInputAvaliable(ByteBuffer inData, int[] size, long[] timeUs, int remaining)
        {

        }
        @Override
        public void onGetData(ByteBuffer encodedData, MediaCodec.BufferInfo bufferInfo) {

            mLastWriteTime = bufferInfo.presentationTimeUs;
            mMuxerCore.waitToWrite();
            mMuxerCore.writeSample(encodedData,bufferInfo,true);
        }

    }
}
