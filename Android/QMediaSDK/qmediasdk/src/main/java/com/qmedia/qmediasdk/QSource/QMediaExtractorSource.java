package com.qmedia.qmediasdk.QSource;

import android.content.res.AssetFileDescriptor;
import android.graphics.SurfaceTexture;
import android.media.AudioFormat;
import android.media.MediaExtractor;
import android.media.MediaFormat;
import android.opengl.GLES20;
import android.view.Surface;

import com.qmedia.qmediasdk.QAudio.QAudioFrame;
import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QCommon.gles.GlUtil;
import com.qmedia.qmediasdk.QCommon.media.HardwareDecoder;
import com.qmedia.qmediasdk.QCommon.QGLContext;
import com.qmedia.qmediasdk.QGraphic.QVideoFrame;
import com.qmedia.qmediasdk.QMediaSDK;
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
    boolean bReadInAsset = false;

    QVideoTarget mVideoTarget;
    QAudioTarget mAudioTarget;

    boolean mIsPrepare = false;
    boolean mIsStarted = false;
    boolean mIsExtractorEnd = false;
    MediaExtractor mExtractor = null;
    Thread mParseThread = null;

    int mOESTextureid = 0;
    SurfaceTexture mVideoSurfaceTexture = null;
    int mVideoIndex = -1;
    int mAudioIndex = -1;

    final int MAX_STREAM_COUNT = 5;
    Vector<MediaStream> mediaStreams = new Vector<>();
    int streamMap[] = new int[MAX_STREAM_COUNT];

    long mMediaDurationUs = 0;
    long mLastPacketTime = -1;
    final int mPacketCacheConut = 25;

    boolean mEnableVideo = true;
    boolean mEnableAudio = true;

    QRange mTimeRange = new QRange();

    AtomicInteger mTextureImageCount = new AtomicInteger(0);

    public QMediaExtractorSource(String filename, boolean inAsset) {
        this(filename,true,true,inAsset);
    }

    public QMediaExtractorSource(String filename, boolean enableVideo, boolean enableAudio,boolean inAsset) {
        mFileName = filename;
        mEnableVideo = enableVideo;
        mEnableAudio = enableAudio;
        bReadInAsset = inAsset;
        for (int i = 0; i < streamMap.length ; i++) {
            streamMap[i] = -1;
        }
    }

    public String getFileName() { return mFileName; }
    public boolean readInAsset() { return bReadInAsset; }
    public boolean enableVideo() { return mEnableVideo; }
    public boolean enableAudio() { return mEnableAudio; }

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
                    Thread.sleep(200);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                continue;
            }

            synchronized (this){
                if ( mIsExtractorEnd) {
                    //wait for decode finished
                    for (MediaStream stream : mediaStreams) {
                        if (!stream.isEnd() && stream.mPacketQueue.size() == 0)
                            stream.mPacketQueue.add(null);
                    }
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    continue;
                }

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
                        stream.lastPacketTimestamp = pts;
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

        mExtractor = new MediaExtractor();
        try {
            if (!bReadInAsset) {
                File videoFile = new File(mFileName);
                mExtractor.setDataSource(videoFile.getAbsolutePath());
            }else {
                AssetFileDescriptor assetFileDescriptor = QMediaSDK.getContext().getAssets().openFd(mFileName);
                mExtractor.setDataSource(assetFileDescriptor.getFileDescriptor(),assetFileDescriptor.getStartOffset(),assetFileDescriptor.getLength());
            }

            int numTracks = mExtractor.getTrackCount();
            for (int i = 0; i < numTracks && i < MAX_STREAM_COUNT; i++) {
                MediaFormat format = mExtractor.getTrackFormat(i);
                MediaStream stream = new MediaStream(format, i);
                if (! stream.isVaild())
                    continue;

                //check enable stream
                if ((!mEnableVideo && !stream.mIsAudio) || (!mEnableAudio && stream.mIsAudio))
                    continue;

                mediaStreams.add(stream);
                streamMap[i] = mediaStreams.size() - 1;
                mMediaDurationUs = Math.max(mMediaDurationUs, stream.mDurationUs);

                if (stream.mVideoDescribe != null) {
                    mVideoIndex = i;
                    mExtractor.selectTrack(mVideoIndex);
                    //TODO: create SurfaceTexture for HardWare Decoder
                    {
                        QGLContext qglContext = QGLContext.shared();
                        synchronized (qglContext) {
                            qglContext.useAsCurrentContext();
                            mOESTextureid = qglContext.createTextureOES();
                            GlUtil.checkGlError("createTextureOES");
                            mVideoSurfaceTexture = new SurfaceTexture(mOESTextureid);
                            qglContext.unUseCurrent();
                        }
                        mVideoSurfaceTexture.setOnFrameAvailableListener(this);
                    }
                }
                else if (stream.mAudioDescribe != null) {
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

            mTimeRange.start = startMSec;
            mTimeRange.end = endMSec;

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
            mParseThread = new Thread(this, "ParseThread");
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
        if (timeMs >= mTimeRange.end) {
            return false;
        }
        mTimeRange.start = timeMs;
        synchronized (this) {
            mLastPacketTime = timeMs;

            for (MediaStream stream : mediaStreams){
                stream.mFrameQueue.setAbort(false);
                stream.flush();
            }

            mIsExtractorEnd = false;
            decodeTo(timeMs, true);
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
        if (videoStream.mFrameQueue.size() <=0 && videoStream.isEnd())
            return null;
        HardwareDecoder.DecodedFrame decodedFrame = videoStream.mFrameQueue.remove();
        QVideoFrame videoFrame = null;
        if (decodedFrame != null) {
            videoFrame = new QVideoFrame(mOESTextureid, decodedFrame.width, decodedFrame.height , QVideoFrame.TEXTURE_OES, decodedFrame.mTimeMs);
            decodedFrame.updateImage(true);
            GLES20.glFinish();
            mVideoSurfaceTexture.updateTexImage();
        }
        return videoFrame;
    }

    @Override
    public QAudioFrame readNextAudioFrame() {
        MediaStream audioStream = mediaStreams.get(streamMap[mAudioIndex]);
        if (audioStream.mFrameQueue.size() <=0 && audioStream.isEnd())
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
//        Log.i(TAG, "onFrameAvailable count " + mTextureImageCount.incrementAndGet());
    }

    boolean decodeTo(long timeMs , boolean isForce) {
        boolean bRet = false;
        int flush_packet_count = 15;

        //find video stream
        MediaStream videoStream = null;
        if (mVideoIndex >= 0)
            videoStream = mediaStreams.get(streamMap[mVideoIndex]);

        if (mVideoIndex >= 0) { //if has video stream
            mExtractor.seekTo(timeMs * 1000, MediaExtractor.SEEK_TO_PREVIOUS_SYNC);
        }
        else
            mExtractor.seekTo(timeMs * 1000, MediaExtractor.SEEK_TO_CLOSEST_SYNC);

        for (MediaStream stream : mediaStreams) {
            stream.waitForFlush();
            if (isForce) //for precision
                stream.setStartTimeLimit(timeMs);
            else //for ambiguous, use first packet timestamp
                stream.setStartTimeLimit(mExtractor.getSampleTime() / 1000);
        }

        while (flush_packet_count > 0) {
            int track_index = mExtractor.getSampleTrackIndex();
            int flag = mExtractor.getSampleFlags();
            long pts = mExtractor.getSampleTime();
            if ((track_index >= 0) && (streamMap[track_index] >= 0)) {
                MediaStream stream = mediaStreams.get(streamMap[track_index]);
                ByteBuffer readBuffer = ByteBuffer.allocate(stream.getMaxInputBufferSize());
                int sampleSize = mExtractor.readSampleData(readBuffer, 0);
                if (sampleSize > 0) {
                    HardwareDecoder.EncodedPacket packet = new HardwareDecoder.EncodedPacket(readBuffer, sampleSize, pts, pts, flag);
                    stream.lastPacketTimestamp = pts;
                    stream.mPacketQueue.add(packet);
                }
            }else if (track_index == -1) {
                for (MediaStream stream : mediaStreams) {
                    if (stream.lastPacketTimestamp == -1)
                        stream.setIsEnd(true);
                    else
                        stream.mPacketQueue.add(null);
                }
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                flush_packet_count --;
            }

            mExtractor.advance();

            //check if deocde finish
            if (videoStream != null) { //wait for first video decodeframe
                if (videoStream.mFrameQueue.size() > 0) {
                    bRet = true;
                    break;
                }
            } else { //wait for first decodeframe
                for (MediaStream stream : mediaStreams) {
                    if (stream.mFrameQueue.size() > 0) {
                        return true;
                    }
                }
            }
        }
        return bRet;
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
