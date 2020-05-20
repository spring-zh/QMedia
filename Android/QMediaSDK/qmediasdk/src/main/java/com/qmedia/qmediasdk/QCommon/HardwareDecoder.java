package com.qmedia.qmediasdk.QCommon;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;

public class HardwareDecoder {
    private static  String TAG = "MediaDecoder";

    public interface DecodecFrameListener {
        void OnDecodecFrame(DecodedImage decodedImage);
    }

    static public class EncodedPacket {
        public EncodedPacket(ByteBuffer buffer, int size, long pts, long dts){
            this.buffer = buffer;
            this.size = size;
            this.dts = this.pts = pts;
            this.dts = dts;
        }
        public ByteBuffer buffer;
        public int size;
        public long pts;
        public long dts;
    }

    static public class DecodedImage {
        public DecodedImage(@NonNull HardwareDecoder decoder, int imgIndex, long timeMs, int flags){
            mDecoder = decoder;
            mIndex = imgIndex;
            mTimeMs = timeMs;
            mFlags = flags;
        }
        public boolean updateImage(boolean render){
            return mDecoder.updateImage(this, render);
        }
        private HardwareDecoder mDecoder;
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

    static public class DecodedAudioBuffer extends DecodedImage {
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
    private boolean mAbout = true;
    private DecodecFrameListener mListener;
    private boolean mUseBuffer = true;
    private boolean mIsAudio;

    public HardwareDecoder(boolean isAudio){
        mIsAudio = isAudio;
    }

    public void setListener(DecodecFrameListener listener){
        mListener = listener;
    }

    public boolean start(MediaFormat format, Surface surface){
        String mime = format.getString(MediaFormat.KEY_MIME);
        synchronized (this){
            try {
                mDecoder = MediaCodec.createDecoderByType(mime);
            }
            catch (Exception e){
                e.printStackTrace();
                Log.e(TAG, "createDecoderByType fail mime = " + mime);
                return false;
            }

            try {
                mDecoder.configure(format, surface, null, 0);
                mDecoder.start();
                mStarting = true;
                mAbout = false;
                if(surface != null)
                    mUseBuffer = false;
                return true;
            } catch (Exception e) {
                e.printStackTrace();
                Log.e(TAG, "configure fail");
                stop();
                return false;
            }
        }
    }

    public void stop(){
        mAbout = true;
        if (mDecoder != null){
            mDecoder.stop();
        }
        mStarting = false;
    }

    public void flush(){
        if (mDecoder != null){
            mDecoder.flush();
        }
    }

    public void release(){
        if (mDecoder != null){
            mDecoder.release();
        }
        mStarting = false;
        mAbout = true;
    }

    public boolean decodePacket(EncodedPacket packet){
        if (!mStarting || mDecoder == null)
            return false;

        ByteBuffer bBufIn = packet.buffer;

        try {

            ByteBuffer[] inputBuffers = mDecoder.getInputBuffers();
            ByteBuffer[] outputBuffers = mDecoder.getOutputBuffers();

//            long realTimeUs = 0;
            MediaCodec.BufferInfo info = new MediaCodec.BufferInfo();
            if (bBufIn != null){
                int index = -1;
                while (!mAbout && (index = mDecoder.dequeueInputBuffer(1000)) < 0){
                    int outIndex = mDecoder.dequeueOutputBuffer(info, 1000);
                    switch (outIndex) {
                        case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED:
                            inputBuffers = mDecoder.getInputBuffers();
                            break;
                        case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED:
                            MediaFormat mediaFormat= mDecoder.getOutputFormat();
                            Log.e(TAG,"KEY_COLOR_FORMAT =  " + mediaFormat.getInteger(MediaFormat.KEY_COLOR_FORMAT));
                            break;
                        case MediaCodec.INFO_TRY_AGAIN_LATER:
                            break;
                        default:
                            checkOutputAndCallback(outIndex,outputBuffers,info);
                    }

                    Thread.sleep(50);//don't dequeue buffer too fast
                }

                if(0 <= index)
                {
                    ByteBuffer buffer = inputBuffers[index];// mDecoder.getInputBuffer(index);
                    buffer.put(bBufIn);
                    mDecoder.queueInputBuffer(index, 0, bBufIn.limit(), packet.pts, 0);
                }
            }

            int outIndex = mDecoder.dequeueOutputBuffer(info, 1000);
            switch (outIndex) {
                case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED:
                    outputBuffers = mDecoder.getOutputBuffers();
                    return false;
                case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED:
                    MediaFormat mediaFormat= mDecoder.getOutputFormat();
                    Log.e(TAG,"KEY_COLOR_FORMAT =  " + mediaFormat.getInteger(MediaFormat.KEY_COLOR_FORMAT));
                    return false;
                case MediaCodec.INFO_TRY_AGAIN_LATER:
                    return false;
                default:
                    checkOutputAndCallback(outIndex,outputBuffers,info);
            }

        } catch (Exception e) {
            return false;
        }

        return true;
    }

    private void checkOutputAndCallback(int outIndex, ByteBuffer[] outputBuffers, MediaCodec.BufferInfo info){
        long realTimeUs = info.presentationTimeUs;
        realTimeUs = realTimeUs < 0 ? 0 : realTimeUs;

        if (info.size <= 0){
            //end of stream

        }else if(mListener != null) {

            MediaFormat format = mDecoder.getOutputFormat();
            if (mIsAudio){
                ByteBuffer cachebuffer = ByteBuffer.allocate(info.size);
                cachebuffer.clear();
                cachebuffer.put(outputBuffers[outIndex]);
                DecodedAudioBuffer audioBuffer = new DecodedAudioBuffer(this,outIndex,realTimeUs,info.flags,cachebuffer);
                audioBuffer.mChannels = format.getInteger(MediaFormat.KEY_CHANNEL_COUNT);
                audioBuffer.mSamplerate = format.getInteger(MediaFormat.KEY_SAMPLE_RATE);
                audioBuffer.mAudioFmt = format.getInteger(MediaFormat.KEY_PCM_ENCODING);
                audioBuffer.size = info.size;
                mListener.OnDecodecFrame(audioBuffer);
                mDecoder.releaseOutputBuffer(outIndex,false);
            }else {
                DecodedImage decodedImage = new DecodedImage(this,outIndex,realTimeUs,info.flags);
                decodedImage.width = format.getInteger(MediaFormat.KEY_WIDTH);
                decodedImage.height = format.getInteger(MediaFormat.KEY_HEIGHT);
                decodedImage.rotation = format.getInteger(MediaFormat.KEY_ROTATION);
                if (mUseBuffer){
                    ByteBuffer cachebuffer = ByteBuffer.allocate(info.size);
                    cachebuffer.clear();
                    cachebuffer.put(outputBuffers[outIndex]);
                    decodedImage.buffer = cachebuffer;
                    decodedImage.isbuffer = true;
                    decodedImage.size = info.size;
                }
                mListener.OnDecodecFrame(decodedImage);
            }

            return;
        }
        mDecoder.releaseOutputBuffer(outIndex,false);
    }

    private boolean updateImage(DecodedImage decodedImage, boolean brender){
        if (decodedImage.mIndex >= 0 && !decodedImage.isbuffer){
            try {
                mDecoder.releaseOutputBuffer(decodedImage.mIndex,brender);
                decodedImage.mIndex = -1;
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
