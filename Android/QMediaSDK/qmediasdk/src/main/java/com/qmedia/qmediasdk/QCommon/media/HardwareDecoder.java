package com.qmedia.qmediasdk.QCommon.media;

import android.media.AudioFormat;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.Surface;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

public class HardwareDecoder {
    private static  String TAG = "MediaDecoder";

    public interface DecodecFrameListener {
        void OnDecodecFrame(DecodedFrame decodedFrame);
        void OnDecodecEnd();
    }

    static public class EncodedPacket {
        public EncodedPacket(ByteBuffer buffer, int size, long pts, long dts, int flag){
            this.buffer = buffer;
            this.size = size;
            this.dts = this.pts = pts;
            this.dts = dts;
            this.flag = flag;
        }
        public ByteBuffer buffer;
        public int size;
        public long pts;
        public long dts;
        public int flag;
    }

    static public class DecodedFrame {
        public DecodedFrame(@NonNull HardwareDecoder decoder, int imgIndex, long timeMs, int flags){
            _weakDecoder = new WeakReference<>(decoder);
            mIndex = imgIndex;
            mTimeMs = timeMs;
            mFlags = flags;
        }
        public boolean updateImage(boolean bRender){
            boolean bRet = false;
            if (_weakDecoder.get() != null) {
                bRet = _weakDecoder.get().updateImage(this, bRender);
                mIndex = -1;
            }
            return bRet;
        }
        private WeakReference<HardwareDecoder>  _weakDecoder;
        public int mIndex;
        public long mTimeMs;
        public int mFlags;

        public int width;
        public int height;
        public int rotation;

        public int textureId;
        //if use buffer cache
        public boolean isbuffer;
        public ByteBuffer buffer;
        public int size;
    }

    static public class DecodedAudioBuffer extends DecodedFrame {
        public DecodedAudioBuffer(@NonNull HardwareDecoder decoder, int imgIndex, long timeMs, int flags, ByteBuffer buffer){
            super(decoder,imgIndex,timeMs,flags);
            isbuffer = true;
            this.buffer = buffer;
        }

        public int mChannels;
        public int mAudioFmt;
        public int mSamplerate;

    }

    private MediaCodec mDecoder = null;
    private boolean mStarting = false;
    public boolean mAbout = false;
    private DecodecFrameListener mListener;
    private boolean mUseBuffer = true;
    private boolean mIsAudio;

    public HardwareDecoder(@NonNull DecodecFrameListener listener, boolean isAudio){
        mListener = listener;
        mIsAudio = isAudio;
    }

    public boolean start(MediaFormat format, Surface surface){
        String mime = format.getString(MediaFormat.KEY_MIME);
        try {
            mDecoder = MediaCodec.createDecoderByType(mime);
            if(surface != null) mUseBuffer = false;
            mDecoder.configure(format, surface, null, 0);
            mDecoder.start();
            mStarting = true;
            mAbout = false;
            return true;
        }
        catch (Exception e){
            e.printStackTrace();
            if (mDecoder != null)
                mDecoder.release();
            return false;
        }
    }

    public void stop(){
        mAbout = true;
        if (mDecoder != null) {
            mDecoder.stop();
        }
        mStarting = false;
    }

    public void flush(){
        if (mDecoder != null) {
            mDecoder.flush();
        }
    }

    public void release(){
        if (mDecoder != null) {
            mDecoder.release();
        }
        mStarting = false;
        mAbout = true;
    }

    //input null packet to wait for decoder finish output
    public int decodePacket_direct(@Nullable EncodedPacket packet) {
        if (!mStarting || mDecoder == null)
            return -1;

        int iRet = 0;

        try {
            ByteBuffer[] inputBuffers = mDecoder.getInputBuffers();
            ByteBuffer[] outputBuffers = mDecoder.getOutputBuffers();
            MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();

            ByteBuffer BuffIn = (packet==null? null : packet.buffer);
            int index = mDecoder.dequeueInputBuffer(1000);
            if (index >= 0) {
                iRet = 0;
                if (BuffIn != null) {
                    ByteBuffer buffer = inputBuffers[index];// mDecoder.getInputBuffer(index);
                    buffer.put(BuffIn);
                    mDecoder.queueInputBuffer(index, 0, BuffIn.limit(), packet.pts, 0);
                }else {
                    mDecoder.queueInputBuffer(index, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                }
            }else //decoder is busy
                iRet = -2;

            int outIndex = mDecoder.dequeueOutputBuffer(info, 1000);
            switch (outIndex) {
                case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED:
                    outputBuffers = mDecoder.getOutputBuffers();
                    break;
                case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED:
                    MediaFormat mediaFormat = mDecoder.getOutputFormat();
                    Log.e(TAG, "KEY_COLOR_FORMAT =  " + mediaFormat.getInteger(MediaFormat.KEY_COLOR_FORMAT));
                    break;
                case MediaCodec.INFO_TRY_AGAIN_LATER:
                    break;
                default:
                    checkOutputAndCallback(outIndex, outputBuffers, info);
                    break;
            }

            if (0 != (info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM)){
                //TODO: read end of stream
                mListener.OnDecodecEnd();
            }


        }catch (Exception e) {
            iRet = -1;
        }

        return iRet;
    }

    //input null packet to wait for decoder finish output
    public boolean decodePacket(@Nullable EncodedPacket packet){
        if (!mStarting || mDecoder == null)
            return false;

        try {

            ByteBuffer[] inputBuffers = mDecoder.getInputBuffers();
            ByteBuffer[] outputBuffers = mDecoder.getOutputBuffers();
            MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();

            ByteBuffer BuffIn = (packet==null? null : packet.buffer);

            int index = -1;
            while (!mAbout && (index = mDecoder.dequeueInputBuffer(1000)) < 0) {
                int outIndex = mDecoder.dequeueOutputBuffer(info, 1000);
                switch (outIndex) {
                    case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED:
                        inputBuffers = mDecoder.getInputBuffers();
                        break;
                    case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED:
                        MediaFormat mediaFormat = mDecoder.getOutputFormat();
                        Log.e(TAG, "KEY_COLOR_FORMAT =  " + mediaFormat.getInteger(MediaFormat.KEY_COLOR_FORMAT));
                        break;
                    case MediaCodec.INFO_TRY_AGAIN_LATER:
                        break;
                    default:
                        checkOutputAndCallback(outIndex, outputBuffers, info);
                }

                if (0 != (info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM)){
                    //TODO: read end of stream
                    mListener.OnDecodecEnd();
                    return false;
                }else if (packet == null)
                    break;

                Thread.sleep(50);//don't dequeue buffer too fast
            }

            if (0 <= index) {
                if (BuffIn != null) {
                    ByteBuffer buffer = inputBuffers[index];// mDecoder.getInputBuffer(index);
                    buffer.put(BuffIn);
                    mDecoder.queueInputBuffer(index, 0, BuffIn.limit(), packet.pts, 0);
                }else {
                    mDecoder.queueInputBuffer(index, 0, 0, 0, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                }
            }

            int outIndex = mDecoder.dequeueOutputBuffer(info, 1000);
            switch (outIndex) {
                case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED:
                    outputBuffers = mDecoder.getOutputBuffers();
                    return false;
                case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED:
                    MediaFormat mediaFormat = mDecoder.getOutputFormat();
                    Log.e(TAG, "KEY_COLOR_FORMAT =  " + mediaFormat.getInteger(MediaFormat.KEY_COLOR_FORMAT));
                    return false;
                case MediaCodec.INFO_TRY_AGAIN_LATER:
                    return false;
                default:
                    checkOutputAndCallback(outIndex, outputBuffers, info);
            }

            if (0 != (info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM)){
                //TODO: read end of stream
                mListener.OnDecodecEnd();
            }

        } catch (Exception e) {
            return false;
        }

        return true;
    }

    private DecodedAudioBuffer makeAudioFrame(int outIndex, ByteBuffer[] outputBuffers, MediaCodec.BufferInfo info ,MediaFormat format, long realTimeMs)
    {
        ByteBuffer cachebuffer = ByteBuffer.allocateDirect(info.size);
        cachebuffer.clear();
        cachebuffer.put(outputBuffers[outIndex]);
        DecodedAudioBuffer audioBuffer = new DecodedAudioBuffer(this, outIndex,realTimeMs,info.flags,cachebuffer);
        audioBuffer.mChannels = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
        audioBuffer.mSamplerate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE);
        audioBuffer.mAudioFmt = format.containsKey(MediaFormat.KEY_PCM_ENCODING)?
                format.getInteger(MediaFormat.KEY_PCM_ENCODING) : AudioFormat.ENCODING_PCM_16BIT;
        audioBuffer.size = info.size;
        return audioBuffer;
    }

    private DecodedFrame makeVideoFrame(int outIndex, ByteBuffer[] outputBuffers, MediaCodec.BufferInfo info ,MediaFormat format, long realTimeMs)
    {
        DecodedFrame decodedFrame = new DecodedFrame(this, outIndex,realTimeMs,info.flags);
        decodedFrame.width = format.getInteger(MediaFormat.KEY_WIDTH);
        decodedFrame.height = format.getInteger(MediaFormat.KEY_HEIGHT);
        decodedFrame.rotation = format.containsKey(MediaFormat.KEY_ROTATION)?
                format.getInteger(MediaFormat.KEY_ROTATION) : 0;
        if (mUseBuffer){
            ByteBuffer cachebuffer = ByteBuffer.allocateDirect(info.size);
            cachebuffer.clear();
            cachebuffer.put(outputBuffers[outIndex]);
            decodedFrame.buffer = cachebuffer;
            decodedFrame.isbuffer = true;
            decodedFrame.size = info.size;
            decodedFrame.mIndex = -1;
        }
        return decodedFrame;
    }

    private void checkOutputAndCallback(int outIndex, ByteBuffer[] outputBuffers, MediaCodec.BufferInfo info){
        long realTimeMs = info.presentationTimeUs / 1000;
        realTimeMs = realTimeMs < 0 ? 0 : realTimeMs;

        if (info.size <= 0 || outIndex < 0){
            //end of stream
            return;
        }
        DecodedFrame outframe;
        if (mIsAudio) {
            outframe = makeAudioFrame(outIndex,outputBuffers,info, mDecoder.getOutputFormat(),realTimeMs);
        }else
            outframe = makeVideoFrame(outIndex,outputBuffers,info, mDecoder.getOutputFormat(),realTimeMs);

        mListener.OnDecodecFrame(outframe);
    }

    private boolean updateImage(DecodedFrame decodedFrame, boolean brender){
        if (decodedFrame.mIndex >= 0){
            try {
                mDecoder.releaseOutputBuffer(decodedFrame.mIndex,brender);
                decodedFrame.mIndex = -1;
                return true;
            }catch (IllegalStateException e) {
                e.printStackTrace();
            }
        }
        return false;
    }

    private boolean isColorFormatSupported(int colorFormat, MediaCodecInfo.CodecCapabilities caps) {
        for (int c : caps.colorFormats) {
            if (c == colorFormat) {
                return true;
            }
        }
        return false;
    }
}
