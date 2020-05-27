package com.qmedia.qmediasdk.QSource;

import android.graphics.SurfaceTexture;
import android.media.AudioFormat;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.opengl.GLES20;
import android.util.Log;
import android.view.Surface;

import com.qmedia.qmediasdk.QAudio.QAudioFrame;
import com.qmedia.qmediasdk.QCommon.HardwareDecoder;
import com.qmedia.qmediasdk.QCommon.QGLContext;
import com.qmedia.qmediasdk.QGraphic.QVideoFrame;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicInteger;

public class QMediaExtractorSource implements QMediaSource ,Runnable, SurfaceTexture.OnFrameAvailableListener{

    private static final String TAG = "QMediaExtractorSource";

    String mFileName;

    QVideoTarget mVideoTarget;
    QAudioTarget mAudioTarget;

    boolean mIsPrepare = false;
    boolean mIsStarted = false;
    boolean mIsExtractorEnd = false;
    MediaExtractor mExtractor = null;
    Thread mParseThread = null;

    int mOESTextureid = 0;
    SurfaceTexture mVideoSurfaceTexture = null;
    MediaStream mVideoStream = null;
    MediaStream mAudioStream = null;
    int mVideoIndex = -1;
    int mAudioIndex = -1;

    long mMediaDurationUs = 0;
    long mLastPacketTime = -1;
    final int mPacketCacheConut = 25;

    AtomicInteger mTextureImageCount = new AtomicInteger(0);

    public QMediaExtractorSource(String filename) {
        mFileName = filename;
    }

    @Override
    public void setVideoTarget(QVideoTarget videoTarget) {
        mVideoTarget = videoTarget;
    }

    @Override
    public void setAudioTarget(QAudioTarget audioTarget) {
        mAudioTarget = audioTarget;
    }

    @Override
    public QVideoTarget getVideoTarget() {
        return mVideoTarget;
    }

    @Override
    public QAudioTarget getAudioTarget() {
        return mAudioTarget;
    }

    @Override
    public QVideoDescribe getVideoDescribe() {
        if (mVideoStream != null)
            return mVideoStream.mVideoDescribe;
        return null;
    }

    @Override
    public QAudioDescribe getAudioDescribe() {
        if (mAudioStream != null)
            return mAudioStream.mAudioDescribe;
        return null;
    }

    @Override
    public void run() {
        while (mIsStarted) {

            boolean bPacketOverCache = true;
            if (mVideoStream != null)
                bPacketOverCache &= (mVideoStream.mPacketQueue.size() >= mPacketCacheConut);
            if (mAudioStream != null)
                bPacketOverCache &= (mAudioStream.mPacketQueue.size() >= mPacketCacheConut);

            if (bPacketOverCache || mIsExtractorEnd) {
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                continue;
            }

            synchronized (this){
                int track_index = mExtractor.getSampleTrackIndex();
                int flag = mExtractor.getSampleFlags();
                long pts = mExtractor.getSampleTime();
                if (track_index == mVideoIndex) {
                    ByteBuffer readBuffer = ByteBuffer.allocate(mVideoStream.getMaxInputBufferSize());
                    int sampleSize = mExtractor.readSampleData(readBuffer, 0);
                    HardwareDecoder.EncodedPacket packet = new HardwareDecoder.EncodedPacket(readBuffer, sampleSize, pts, pts, flag);
                    if (sampleSize > 0) {
                        mVideoStream.mPacketQueue.add(packet);
                    }
                }else if (track_index == mAudioIndex) {
                    ByteBuffer readBuffer = ByteBuffer.allocate(mAudioStream.getMaxInputBufferSize());
                    int sampleSize = mExtractor.readSampleData(readBuffer, 0);
                    HardwareDecoder.EncodedPacket packet = new HardwareDecoder.EncodedPacket(readBuffer, sampleSize, pts, pts, flag);
                    if (sampleSize > 0) {
                        mAudioStream.mPacketQueue.add(packet);
                    }
                }else if (track_index == -1) {
                    mIsExtractorEnd = true;
                }

                if (! mExtractor.advance()){
                    mVideoStream.mPacketQueue.add(null);
                    mAudioStream.mPacketQueue.add(null);
                    mIsExtractorEnd = true;
                }
            }//synchronized
        }
    }

    @Override
    public boolean load() {
        if(mIsPrepare)
            return true;
        File videoFile = new File(mFileName);
        mExtractor = new MediaExtractor();
        try {
            mExtractor.setDataSource(videoFile.getAbsolutePath());
            int numTracks = mExtractor.getTrackCount();
            for (int i = 0; i < numTracks; i++) {
                MediaFormat format = mExtractor.getTrackFormat(i);
                MediaStream stream = new MediaStream(format, i);
                mMediaDurationUs = Math.max(mMediaDurationUs, stream.mDurationUs);
                if (stream.mVideoDescribe != null) {
                    mVideoStream = stream;
                    mVideoIndex = i;
                    mExtractor.selectTrack(mVideoIndex);
                    //TODO: create SurfaceTexture for HardWare Decoder
                    {
                        QGLContext qglContext = QGLContext.shared();
                        synchronized (qglContext) {
                            qglContext.unUseCurrent();
                            mOESTextureid = qglContext.createTextureOES();
                            mVideoSurfaceTexture = new SurfaceTexture(mOESTextureid);
                            mVideoSurfaceTexture.setOnFrameAvailableListener(this);
                            qglContext.unUseCurrent();
                        }
                    }
                }
                else if (stream.mAudioDescribe != null) {
                    mAudioStream = stream;
                    mAudioIndex = i;
                    mExtractor.selectTrack(mAudioIndex);
                }
            }

        } catch (IOException e) {
            e.printStackTrace();
            mIsPrepare = false;
        }
        mIsPrepare = true;
        return true;
    }

    @Override
    public boolean unload() {
        if (mIsPrepare) {
            if (mExtractor != null){
                mExtractor.release();
                mExtractor = null;
            }
            if (mVideoStream != null)
                mVideoStream.release();
            if (mAudioStream != null)
                mAudioStream.release();

            if (mVideoSurfaceTexture != null){
                {
                    QGLContext qglContext = QGLContext.shared();
                    synchronized (qglContext) {
                        qglContext.useAsCurrentContext();
                        if (mOESTextureid > 0) {
                            int[] textures = new int[1];
                            textures[0] = mOESTextureid;
                            GLES20.glDeleteTextures(1, textures, 0);
                            mOESTextureid = 0;
                        }
                        qglContext.unUseCurrent();
                    }
                }

                mVideoSurfaceTexture.release();
                mVideoSurfaceTexture = null;
            }

            mVideoIndex = -1;
            mAudioIndex = -1;
            mIsPrepare = false;
        }
        return false;
    }

    @Override
    public boolean isInit() {
        return mIsPrepare;
    }

    @Override
    public boolean start(long startMSec, long endMSec) {
        if (mIsPrepare) {
            if (mIsStarted)
                return true;

            mIsExtractorEnd = false;
            mExtractor.seekTo(startMSec,MediaExtractor.SEEK_TO_PREVIOUS_SYNC);
            mLastPacketTime = startMSec;

            if (mVideoStream != null) {
                mVideoStream.start(new Surface(mVideoSurfaceTexture));
            }
            if (mAudioStream != null)
                mAudioStream.start(null);

            mIsStarted = true;
            mParseThread = new Thread(this);
            mParseThread.start();
            return true;
        }
        return false;
    }

    @Override
    public void stop() {
        if (mIsStarted){

            mIsStarted = false;

            if (mParseThread != null){
                try {
                    mParseThread.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                mParseThread = null;
            }

            if (mAudioStream != null)
                mAudioStream.stop();
            if (mVideoStream != null)
                mVideoStream.stop();
        }
    }

    @Override
    public boolean seekTo(long timeMs) {
        if (!mIsStarted)
            return false;
        synchronized (this) {
            mLastPacketTime = timeMs;
            if (mVideoStream != null) {
                mVideoStream.setStartTimeLimit(timeMs);
                mVideoStream.flush();
                if (mAudioStream != null) {
                    mAudioStream.setStartTimeLimit(timeMs);
                    mAudioStream.flush();
                }
                mIsExtractorEnd = false;
                mExtractor.seekTo(timeMs * 1000, MediaExtractor.SEEK_TO_PREVIOUS_SYNC);
            }
        }
        return true;
    }

    @Override
    public boolean isEOF() {
        return mIsExtractorEnd;
    }

    @Override
    public boolean isStarted() {
        return mIsStarted;
    }

    @Override
    public long getMediaDuration() {
        return mMediaDurationUs / 1000;
    }

    @Override
    public QVideoFrame readNextVideoFrame() {
        HardwareDecoder.DecodedFrame decodedFrame = mVideoStream.mFrameQueue.remove();
        QVideoFrame videoFrame = null;
        if (decodedFrame != null) {
            videoFrame = new QVideoFrame(mOESTextureid, decodedFrame.width, decodedFrame.height , QVideoFrame.TEXTURE_OES, decodedFrame.mTimeMs);
            decodedFrame.updateImage(true);
            mVideoSurfaceTexture.updateTexImage();
        }
        return videoFrame;
    }

    @Override
    public QAudioFrame readNextAudioFrame() {
        HardwareDecoder.DecodedAudioBuffer decodedAudioBuffer = (HardwareDecoder.DecodedAudioBuffer) mAudioStream.mFrameQueue.remove();
        QAudioFrame audioFrame = null;
        if (decodedAudioBuffer != null) {
            int formatPerSample = getAudioFormatBytes(decodedAudioBuffer.mAudioFmt);
            int samples = decodedAudioBuffer.size / (decodedAudioBuffer.mChannels * formatPerSample);
            audioFrame = new QAudioFrame(decodedAudioBuffer.buffer, decodedAudioBuffer.mChannels, decodedAudioBuffer.mSamplerate ,
                    samples,formatPerSample*8, decodedAudioBuffer.size, decodedAudioBuffer.mTimeMs);
        }
        return audioFrame;
    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {
        Log.i(TAG, "onFrameAvailable count " + mTextureImageCount.incrementAndGet());
    }

    int getAudioFormatBytes(int audioFormat) {
        int audioBytes = 2;
        switch (audioFormat) {
            case AudioFormat.ENCODING_PCM_8BIT:
                audioBytes = 1;
                break;
            case AudioFormat.ENCODING_PCM_16BIT:
                audioBytes = 2;
                break;
            case AudioFormat.ENCODING_PCM_FLOAT:
                audioBytes = 4;
                break;
        }
        return audioBytes;
    }

}
