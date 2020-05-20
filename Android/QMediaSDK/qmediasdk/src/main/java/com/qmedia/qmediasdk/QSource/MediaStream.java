package com.qmedia.qmediasdk.QSource;

import android.media.AudioFormat;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.view.Surface;

import com.qmedia.qmediasdk.QCommon.FrameCacheQueue;
import com.qmedia.qmediasdk.QCommon.HardwareDecoder;
import com.qmedia.qmediasdk.QSource.QMediaDescribe;

import java.util.LinkedList;

public class MediaStream implements Runnable, HardwareDecoder.DecodecFrameListener {

    public boolean mbStart = false;
    public long mStartTimeLimit;
    public int mTrackId;

    public boolean mIsAudio;
    public long mDurationUs;
    //for video
    public QVideoDescribe mVideoDescribe;
    //for audio
    public QAudioDescribe mAudioDescribe;

    public PacketQueue mPacketQueue;
    public FrameCacheQueue mFrameQueue;

    public MediaFormat mFormat;
    public HardwareDecoder mMediaDecoder;
    public int mMaxOutputFrameNum = 4;
    private Thread mDecodeThread;

    public MediaStream(MediaFormat format, int index) {
        mFormat = format;
        String mime = format.getString(MediaFormat.KEY_MIME);
        if (mime.startsWith("video/")) {
            mVideoDescribe = MediaFormatToVideoDescribe(format);
            mIsAudio = false;
        }else if (mime.startsWith("audio/")){
            mAudioDescribe = MediaFormatToAudioDescribe(format);
            mIsAudio = true;
        }else
            return;

        mDurationUs = format.getLong(MediaFormat.KEY_DURATION);
        mTrackId = index;
        //init
        mStartTimeLimit = 0;
        mTrackId = -1;
        mPacketQueue = new PacketQueue();
        mFrameQueue = new FrameCacheQueue();

        //init decoder
        mMediaDecoder = new HardwareDecoder(mIsAudio);
        mMediaDecoder.setListener(this);
    }

    public void setMaxOutputFrameNum(int num){
        mMaxOutputFrameNum = num;
    }

    //call this for limit output frame'pts by seek
    public void setStartTimeLimit(long startTimeLimit){ mStartTimeLimit = startTimeLimit;}

    //start decoded thread
    public void start(Surface output){
        if (!mbStart)
        {
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
        synchronized (mMediaDecoder) {
            mPacketQueue.clear();
            mMediaDecoder.flush();
        }
        mFrameQueue.clear();
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
            HardwareDecoder.EncodedPacket packet = mPacketQueue.remove();
            if (packet != null) {
                synchronized (mMediaDecoder) {
                    mMediaDecoder.decodePacket(packet);
                }
            }else //abort
                break;
        }
    }

    //this queue use to cache packets
    public class PacketQueue extends LinkedList<HardwareDecoder.EncodedPacket> {
        @Override
        public boolean add(HardwareDecoder.EncodedPacket packet)
        {
            synchronized (this) {
                boolean bRet = super.add(packet);
                this.notify();
                return bRet;
            }
        }

        @Override
        public HardwareDecoder.EncodedPacket remove() {
            HardwareDecoder.EncodedPacket packet = null;
            synchronized (this) {

                while (super.size() <= 0){
                    try {
                        this.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }

                packet = super.remove();
            }
            return packet;
        }

        @Override
        public void clear(){
            synchronized (this) {
                super.clear();
            }
        }
    }

    @Override
    public void OnDecodecFrame(HardwareDecoder.DecodedImage decodedImage) {
        //decoded to the seek position
        if (decodedImage.mTimeMs < mStartTimeLimit - 50) {
            decodedImage.updateImage(false);
            return;
        }
        mFrameQueue.add(decodedImage);
    }

    public static QVideoDescribe MediaFormatToVideoDescribe(MediaFormat mediaFormat) {
        int raw_format = QVideoDescribe.QRawVideoFormatHardware;
        switch (mediaFormat.getInteger(MediaFormat.KEY_COLOR_FORMAT)) {
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface:
                raw_format = QVideoDescribe.QRawVideoFormatHardware;
                break;
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar:
                raw_format = QVideoDescribe.QRawVideoFormatI420;
                break;
            case MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420SemiPlanar:
                raw_format = QVideoDescribe.QRawVideoFormatNV12;
                break;
            case MediaCodecInfo.CodecCapabilities.COLOR_Format32bitARGB8888:
                raw_format = QVideoDescribe.QRawVideoFormatARGB;
                break;
        }

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

        return new QVideoDescribe(codec, raw_format, mediaFormat.getInteger(MediaFormat.KEY_ROTATION),
                mediaFormat.getFloat(MediaFormat.KEY_FRAME_RATE),
                mediaFormat.getInteger(MediaFormat.KEY_WIDTH), mediaFormat.getInteger(MediaFormat.KEY_HEIGHT),
                mediaFormat.getInteger(MediaFormat.KEY_BIT_RATE), 1000);
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

        int raw_format = QAudioDescribe.QRawAudioFormatUnknown;
        switch (mediaFormat.getInteger(MediaFormat.KEY_PCM_ENCODING)) {
            case AudioFormat.ENCODING_PCM_16BIT:
                raw_format = QAudioDescribe.QRawAudioFormatS16;
                break;
            case AudioFormat.ENCODING_PCM_8BIT:
                raw_format = QAudioDescribe.QRawAudioFormatU8;
                break;
            case AudioFormat.ENCODING_PCM_FLOAT:
                raw_format = QAudioDescribe.QRawAudioFormatFLOAT;
                break;
        }

        return new QAudioDescribe(codec, raw_format,
                mediaFormat.getInteger(MediaFormat.KEY_SAMPLE_RATE),mediaFormat.getInteger(MediaFormat.KEY_CHANNEL_COUNT),
                mediaFormat.getInteger(MediaFormat.KEY_BIT_RATE));
    }
}
