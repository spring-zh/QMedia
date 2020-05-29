package com.qmedia.qmediasdk.QCommon.media;

import android.media.MediaCodec;
import android.media.MediaFormat;
import android.media.MediaMuxer;
import android.util.Log;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by spring on 2017/5/3.
 */

public class MuxerCore {

    private final String TAG = "MuxerCore";

    private boolean ENABLE_DEBUG = false;

    Object mWaiteLck = new Object();
    boolean mMuxerStarted = false;
    boolean mAbort = false;

    private MediaMuxer mMuxer;
    private int mVideoTrackIndex = -1;
    private int mAudioTrackIndex = -1;
    boolean mbHasAudio = false;

    private long mVideoBaseUs = -1;
    private long mAudioBaseUs = -1;

    private String mFileName;

    String getFileName(){
        return mFileName;
    }

    public MuxerCore(String fileName, boolean hasAudio) throws IOException {
        mMuxer = new MediaMuxer(fileName, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4);
        mFileName = fileName;
        mbHasAudio = hasAudio;
        mMuxerStarted = false;
        mVideoTrackIndex = -1;
        mAudioTrackIndex = -1;
    }

    public void waitToWrite(){
        synchronized (mWaiteLck){
            while (!mMuxerStarted && !mAbort){
                try {
                    mWaiteLck.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void abortWait(){
        mAbort = true;
        synchronized (mWaiteLck) {
            mWaiteLck.notifyAll();
        }
    }

    synchronized public void setTrackFormat(MediaFormat format, boolean isAudio) {
        if (isAudio && mAudioTrackIndex == -1) {
            mAudioTrackIndex = mMuxer.addTrack(format);
        } else if (mVideoTrackIndex == -1) {
            mVideoTrackIndex = mMuxer.addTrack(format);

        }

        if (mVideoTrackIndex != -1) {
            if (mbHasAudio && mAudioTrackIndex == -1)
                return;

            if (ENABLE_DEBUG) Log.d(TAG, "MuxerCore started... ");
            mMuxer.start();
            synchronized (mWaiteLck) {
                mMuxerStarted = true;
                mWaiteLck.notifyAll();
            }
        }
    }

    synchronized public void writeSample(ByteBuffer byteBuf, MediaCodec.BufferInfo bufferInfo, boolean isAudio) {
        if (!mMuxerStarted) {
            //throw new RuntimeException("Muxer not start!!");
            Log.i(TAG,"writer not ready and discard is audio " + isAudio);
            return;
        }
        if (isAudio && mAudioTrackIndex != -1) {
            if (mAudioBaseUs == -1)
                mAudioBaseUs = bufferInfo.presentationTimeUs;
            bufferInfo.presentationTimeUs = bufferInfo.presentationTimeUs - mAudioBaseUs;
            mMuxer.writeSampleData(mAudioTrackIndex, byteBuf, bufferInfo);
            if (ENABLE_DEBUG)
                Log.d(TAG, "write audio SampleData " + bufferInfo.size + " pts " + bufferInfo.presentationTimeUs);
        } else {
            if (mVideoBaseUs == -1)
                mVideoBaseUs = bufferInfo.presentationTimeUs;
            bufferInfo.presentationTimeUs = bufferInfo.presentationTimeUs - mVideoBaseUs;
            mMuxer.writeSampleData(mVideoTrackIndex, byteBuf, bufferInfo);
            if (ENABLE_DEBUG)
                Log.d(TAG, "write video SampleData " + bufferInfo.size + " pts " + bufferInfo.presentationTimeUs);
        }
    }

    public void close() {
        if (mMuxer == null)
            throw new RuntimeException("Muxer is nil !!");

        try{
            if (mMuxerStarted)
                mMuxer.stop();
            mMuxer.release();
        }catch (Exception e){
            e.printStackTrace();
        }

        mMuxerStarted = false;
        mVideoTrackIndex = -1;
        mAudioTrackIndex = -1;
    }
}