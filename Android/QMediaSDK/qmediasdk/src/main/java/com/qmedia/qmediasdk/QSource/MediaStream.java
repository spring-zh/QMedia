package com.qmedia.qmediasdk.QSource;

import android.graphics.SurfaceTexture;
import android.media.AudioFormat;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.opengl.GLES20;
import android.os.Looper;
import android.util.Log;
import android.view.Surface;

import com.qmedia.qmediasdk.QCommon.EncodedPacketQueue;
import com.qmedia.qmediasdk.QCommon.FrameCacheQueue;
import com.qmedia.qmediasdk.QCommon.HardwareDecoder;
import com.qmedia.qmediasdk.QCommon.QGLContext;

import java.util.LinkedList;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Handler;

public class MediaStream implements Runnable, HardwareDecoder.DecodecFrameListener{

    private static final String TAG = "MediaStream";
    public boolean mbStart = false;
    protected long mStartTimeLimit;
    public int mTrackId;

    public boolean mIsAudio;
    public long mDurationUs;
    //for video
    public QVideoDescribe mVideoDescribe;
    //for audio
    public QAudioDescribe mAudioDescribe;

    public EncodedPacketQueue mPacketQueue = new EncodedPacketQueue();
    public FrameCacheQueue mFrameQueue = new FrameCacheQueue();

    protected MediaFormat mFormat;
    protected HardwareDecoder mMediaDecoder;
    public int mMaxOutputFrameNum = 4;
    private Thread mDecodeThread;

    private int maxInputBufferSize = 0;

    public MediaStream(MediaFormat format, int index) {
        mFormat = format;
        String mime = format.getString(MediaFormat.KEY_MIME);
        if (mime.startsWith("video/")) {
            mVideoDescribe = MediaFormatToVideoDescribe(format);
            mIsAudio = false;
        }else if (mime.startsWith("audio/")){
            mFormat.setInteger(MediaFormat.KEY_PCM_ENCODING, AudioFormat.ENCODING_PCM_16BIT);
            mAudioDescribe = MediaFormatToAudioDescribe(format);
            mIsAudio = true;
        }else
            return;
        maxInputBufferSize = format.getInteger(MediaFormat.KEY_MAX_INPUT_SIZE);
        mDurationUs = format.getLong(MediaFormat.KEY_DURATION);
        mTrackId = index;
        //init
        mStartTimeLimit = 0;

        //init decoder
        mMediaDecoder = new HardwareDecoder(this, mIsAudio);
    }

    public int getMaxInputBufferSize() {
        return maxInputBufferSize;
    }

    //call this for limit output frame'pts by seek
    public void setStartTimeLimit(long startTimeLimit){ mStartTimeLimit = startTimeLimit;}

    //start decoded thread
    public void start(Surface output){
        if (!mbStart)
        {
            mFrameQueue.setAbort(false);
            if (!mMediaDecoder.start(mFormat, output))
                return;
            mbStart = true;
            mDecodeThread = new Thread(this);
            mDecodeThread.start();
        }
    }

    //stop decoded thread
    public void stop(){
        mbStart = false;
        mFrameQueue.setAbort(true);
        mPacketQueue.add(null);//add null packet to abort thie waiting for PacketQueue
        if (mDecodeThread != null) {
            try {
                mDecodeThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            mDecodeThread = null;
        }
        mMediaDecoder.stop();
    }

    //flush packets、decoder and output frames data
    public void flush(){
        mPacketQueue.clear();
        mFrameQueue.setAbort(true);
        mFrameQueue.clear();
        mFrameQueue.setAbort(false);
        mMediaDecoder.flush();
    }

    //release just been called after stop
    public void release(){
        if (mbStart)
            return;

        mMediaDecoder.release();
    }

    @Override
    public void run(){
        while (mbStart){
            try {
                // handle output mFrameQueue overload
                while (mFrameQueue.size() > mMaxOutputFrameNum && !mFrameQueue.isAbort()) {
                    //TODO: use notify to replace it
                    Thread.sleep(10);
                }
                if (! mbStart) break;

                HardwareDecoder.EncodedPacket packet = mPacketQueue.remove();
                mMediaDecoder.decodePacket(packet);

            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private int frameCount = 0;
    @Override
    public void OnDecodecFrame(HardwareDecoder.DecodedFrame decodedFrame) {
        //decoded to the seek position
        if (decodedFrame.mTimeMs < mStartTimeLimit - 50) {
            Log.i(TAG, "OnDecodecFrame drop frame " + frameCount);
            frameCount ++;
            decodedFrame.updateImage(false);
            return;
        }
        mFrameQueue.add(decodedFrame);
    }

    @Override
    public void OnDecodecEnd() {

    }

    public static QVideoDescribe MediaFormatToVideoDescribe(MediaFormat mediaFormat) {
        int raw_format = QVideoDescribe.QRawVideoFormatHardware;
//        switch (mediaFormat.getInteger(MediaFormat.KEY_COLOR_FORMAT)) {
//            case MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface:
//                raw_format = QVideoDescribe.QRawVideoFormatHardware;
//                break;
//            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar:
//                raw_format = QVideoDescribe.QRawVideoFormatI420;
//                break;
//            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar:
//                raw_format = QVideoDescribe.QRawVideoFormatNV12;
//                break;
//            case MediaCodecInfo.CodecCapabilities.COLOR_Format32bitARGB8888:
//                raw_format = QVideoDescribe.QRawVideoFormatARGB;
//                break;
//        }

        int codec = QVideoDescribe.QVideoCodecUnknown;
        String mime = mediaFormat.getString(MediaFormat.KEY_MIME);
        if (mime.equals(MediaFormat.MIMETYPE_VIDEO_VP8)) {
            codec = QVideoDescribe.QVideoCodecVP8;
        }else if (mime.equals(MediaFormat.MIMETYPE_VIDEO_VP9)) {
            codec = QVideoDescribe.QVideoCodecVP9;
        }else if (mime.equals(MediaFormat.MIMETYPE_VIDEO_AVC)) {
            codec = QVideoDescribe.QVideoCodecH264;
        }else if (mime.equals(MediaFormat.MIMETYPE_VIDEO_HEVC)) {
            codec = QVideoDescribe.QVideoCodecH265;
        }else if (mime.equals(MediaFormat.MIMETYPE_VIDEO_MPEG4)) {
            codec = QVideoDescribe.QVideoCodecMPEG4;
        }

        int rotation = mediaFormat.containsKey(MediaFormat.KEY_ROTATION)?
                mediaFormat.getInteger(MediaFormat.KEY_ROTATION) : 0;
        float framerate = mediaFormat.containsKey(MediaFormat.KEY_FRAME_RATE)?
                mediaFormat.getInteger(MediaFormat.KEY_FRAME_RATE) : 0;
        int bitrate = mediaFormat.containsKey(MediaFormat.KEY_BIT_RATE)?
                mediaFormat.getInteger(MediaFormat.KEY_BIT_RATE) : 0;
        int width = mediaFormat.getInteger(MediaFormat.KEY_WIDTH);
        int height = mediaFormat.getInteger(MediaFormat.KEY_HEIGHT);

        return new QVideoDescribe(codec, raw_format, rotation,
                framerate,
                width, height,
                bitrate, 1000);
    }

    public static QAudioDescribe MediaFormatToAudioDescribe(MediaFormat mediaFormat) {
        int codec = QAudioDescribe.QAudioCodecUnknown;
        String mime = mediaFormat.getString(MediaFormat.KEY_MIME);
        if (mime.equals(MediaFormat.MIMETYPE_AUDIO_AAC)) {
            codec = QAudioDescribe.QAudioCodecAAC;
        }else if (mime.equals(MediaFormat.MIMETYPE_AUDIO_OPUS)) {
            codec = QAudioDescribe.QAudioCodecOpus;
        }else if (mime.equals(MediaFormat.MIMETYPE_AUDIO_G711_ALAW)) {
            codec = QAudioDescribe.QAudioCodecPcmA;
        }else if (mime.equals(MediaFormat.MIMETYPE_AUDIO_G711_MLAW)) {
            codec = QAudioDescribe.QAudioCodecPcmU;
        }

        int raw_format = QAudioDescribe.QRawAudioFormatS16;
//        switch (mediaFormat.getInteger(MediaFormat.KEY_PCM_ENCODING)) {
//            case AudioFormat.ENCODING_PCM_16BIT:
//                raw_format = QAudioDescribe.QRawAudioFormatS16;
//                break;
//            case AudioFormat.ENCODING_PCM_8BIT:
//                raw_format = QAudioDescribe.QRawAudioFormatU8;
//                break;
//            case AudioFormat.ENCODING_PCM_FLOAT:
//                raw_format = QAudioDescribe.QRawAudioFormatFLOAT;
//                break;
//        }

        int samplerate = mediaFormat.getInteger(MediaFormat.KEY_SAMPLE_RATE);
        int channel_count = mediaFormat.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
        int bitrate = mediaFormat.containsKey(MediaFormat.KEY_BIT_RATE)?
                mediaFormat.getInteger(MediaFormat.KEY_BIT_RATE) : 0;

        return new QAudioDescribe(codec, raw_format,
                samplerate,channel_count, bitrate);
    }
}
