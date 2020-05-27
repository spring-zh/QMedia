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
import java.util.Vector;
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
//    MediaStream mVideoStream = null;
//    MediaStream mAudioStream = null;
    int mVideoIndex = -1;
    int mAudioIndex = -1;

    final int MAX_STREAM_COUNT = 5;
    Vector<MediaStream> mediaStreams = new Vector<>();
    int streamMap[] = new int[MAX_STREAM_COUNT];

    long mMediaDurationUs = 0;
    long mLastPacketTime = -1;
    final int mPacketCacheConut = 25;

    AtomicInteger mTextureImageCount = new AtomicInteger(0);

    public QMediaExtractorSource(String filename) {
        mFileName = filename;
        for (int i = 0; i < streamMap.length ; i++) {
            streamMap[i] = -1;
        }
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
        if (mVideoIndex >= 0)
            return mediaStreams.get(streamMap[mVideoIndex]).mVideoDescribe;
        return null;
    }

    @Override
    public QAudioDescribe getAudioDescribe() {
        if (mAudioIndex >= 0)
            return mediaStreams.get(streamMap[mAudioIndex]).mAudioDescribe;
        return null;
    }

    @Override
    public void run() {
        while (mIsStarted) {

            boolean bPacketOverCache = true;
            for (MediaStream stream : mediaStreams) {
                bPacketOverCache &= (stream.mPacketQueue.size() >= mPacketCacheConut);
            }

            if (bPacketOverCache) {
                try {
                    Thread.sleep(500);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                continue;
            }

            if ( mIsExtractorEnd) {
                for (MediaStream stream : mediaStreams) {
                    if (!stream.isEnd())
                        stream.mPacketQueue.add(null);
                }
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                continue;
            }

            synchronized (this){
                int track_index = mExtractor.getSampleTrackIndex();
                int flag = mExtractor.getSampleFlags();
                long pts = mExtractor.getSampleTime();

                if ((track_index >= 0) && (streamMap[track_index] >= 0)) {
                    MediaStream stream = mediaStreams.get(streamMap[track_index]);
                    ByteBuffer readBuffer = ByteBuffer.allocate(stream.getMaxInputBufferSize());
                    int sampleSize = mExtractor.readSampleData(readBuffer, 0);
                    if (sampleSize > 0) {
                        HardwareDecoder.EncodedPacket packet = new HardwareDecoder.EncodedPacket(readBuffer, sampleSize, pts, pts, flag);
                        stream.mPacketQueue.add(packet);
                    }
                }else if (track_index == -1) {
                    mIsExtractorEnd = true;
                }

                if (! mExtractor.advance()){
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
            for (int i = 0; i < numTracks && i < MAX_STREAM_COUNT; i++) {
                MediaFormat format = mExtractor.getTrackFormat(i);
                MediaStream stream = new MediaStream(format, i);
                if (! stream.isVaild())
                    continue;

                mediaStreams.add(stream);
                streamMap[i] = mediaStreams.size() - 1;
                mMediaDurationUs = Math.max(mMediaDurationUs, stream.mDurationUs);

                if (stream.mVideoDescribe != null) {
//                    mVideoStream = stream;
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
//                    mAudioStream = stream;
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
            for (MediaStream stream : mediaStreams) {
                stream.release();;
            }
            mediaStreams.clear();
            mVideoIndex = mAudioIndex = -1;

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

            for (MediaStream stream : mediaStreams) {
                if (stream.mVideoDescribe != null)
                    stream.start(new Surface(mVideoSurfaceTexture));
                else
                    stream.start(null);
            }

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

            for (MediaStream stream : mediaStreams)
                stream.stop();
        }
    }

    @Override
    public boolean seekTo(long timeMs) {
        if (!mIsStarted)
            return false;
        synchronized (this) {
            mLastPacketTime = timeMs;

            for (MediaStream stream : mediaStreams){
                stream.setStartTimeLimit(timeMs);
                stream.flush();
            }
            mIsExtractorEnd = false;
            mExtractor.seekTo(timeMs*1000, MediaExtractor.SEEK_TO_PREVIOUS_SYNC);
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
        MediaStream videoStream = mediaStreams.get(streamMap[mVideoIndex]);
        if (videoStream.isEnd())
            return null;
        HardwareDecoder.DecodedFrame decodedFrame = videoStream.mFrameQueue.remove();
        QVideoFrame videoFrame = null;
        if (decodedFrame != null) {
            videoFrame = new QVideoFrame(mOESTextureid, decodedFrame.width, decodedFrame.height , QVideoFrame.TEXTURE_OES, decodedFrame.mTimeMs);
//            decodedFrame.updateImage(true);
            mVideoSurfaceTexture.updateTexImage();
        }
        return videoFrame;
    }

    @Override
    public QAudioFrame readNextAudioFrame() {
        MediaStream audioStream = mediaStreams.get(streamMap[mAudioIndex]);
        if (audioStream.isEnd())
            return null;
        HardwareDecoder.DecodedAudioBuffer decodedAudioBuffer = (HardwareDecoder.DecodedAudioBuffer) audioStream.mFrameQueue.remove();
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
