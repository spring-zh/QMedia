package com.qmedia.qmediasdk.QSource;

import android.graphics.SurfaceTexture;
import android.media.AudioFormat;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.opengl.GLES20;
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

    long mMediaDuration = 0;
    long mLastPacketTime = -1;
    final int mMaxCacheFrameConut = 4;

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
            synchronized (this){
                boolean bPacketOverCache = true;
                if (mVideoStream != null)
                    bPacketOverCache &= (mVideoStream.mFrameQueue.size() >= mMaxCacheFrameConut);
                if (mAudioStream != null)
                    bPacketOverCache &= (mAudioStream.mFrameQueue.size() >= mMaxCacheFrameConut);

                if (bPacketOverCache) {
                    try {
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    continue;
                }

                //read encoded packet and send to decoder
                ByteBuffer readBuffer = ByteBuffer.allocateDirect(0);
                readBuffer.clear();

                int sampleSize = mExtractor.readSampleData(readBuffer, 0);
                if (sampleSize > 0) {
                    long pts = mExtractor.getSampleTime();
                    mLastPacketTime = pts;
                    HardwareDecoder.EncodedPacket packet = new HardwareDecoder.EncodedPacket(readBuffer, sampleSize, pts, pts);

                    int track_index = mExtractor.getSampleTrackIndex();
                    if (track_index == mVideoIndex)
                        mVideoStream.mPacketQueue.add(packet);
                    else if (track_index == mAudioIndex)
                        mAudioStream.mPacketQueue.add(packet);

                } else if (sampleSize == -1) {//no more samples are available
                    int track_index = mExtractor.getSampleTrackIndex();
                    mIsExtractorEnd = true;
                }

                mExtractor.advance();
            }
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
                mMediaDuration = Math.max(mMediaDuration, stream.mDurationUs);
                if (stream.mVideoDescribe != null) {
                    mVideoStream = stream;
                    mVideoIndex = i;
                    mExtractor.selectTrack(mVideoIndex);
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
                    qglContext.useAsCurrentContext();
                    if (mOESTextureid > 0){
                        int[] textures = new int[1];
                        textures[0] = mOESTextureid;
                        GLES20.glDeleteTextures( 1, textures , 0);
                        mOESTextureid = 0;
                    }
                    qglContext.unUseCurrent();
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
                {
                    QGLContext qglContext = QGLContext.shared();
                    qglContext.unUseCurrent();
                    mOESTextureid = qglContext.createTextureOES();
                    mVideoSurfaceTexture = new SurfaceTexture(mOESTextureid);
                    qglContext.unUseCurrent();
                }
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
    public boolean seekTo(long timeUs) {
        if (!mIsStarted)
            return false;


        synchronized (this) {
            mLastPacketTime = timeUs;

            if (mVideoStream != null) {
                mVideoStream.flush();
                mVideoStream.setStartTimeLimit(timeUs);
            }
            if (mAudioStream != null) {
                mAudioStream.flush();
                mAudioStream.setStartTimeLimit(timeUs);
            }

            mIsExtractorEnd = false;
            mExtractor.seekTo(timeUs,MediaExtractor.SEEK_TO_PREVIOUS_SYNC);
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
        return mMediaDuration;
    }

    @Override
    public QVideoFrame readNextVideoFrame() {
        HardwareDecoder.DecodedImage decodedImage = mVideoStream.mFrameQueue.remove();
        QVideoFrame videoFrame = null;
        if (decodedImage != null) {
            videoFrame = new QVideoFrame(mOESTextureid, decodedImage.width, decodedImage.height , QVideoFrame.TEXTURE_OES, decodedImage.mTimeMs);
            decodedImage.updateImage(true);
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
