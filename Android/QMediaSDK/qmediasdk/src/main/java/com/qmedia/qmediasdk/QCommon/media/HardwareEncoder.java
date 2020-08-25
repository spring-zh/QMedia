package com.qmedia.qmediasdk.QCommon.media;

import android.annotation.TargetApi;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.media.MediaFormat;
import android.support.annotation.NonNull;
import android.util.Log;
import android.view.Surface;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by spring on 2017/5/3.
 */

public class HardwareEncoder {

    private final String TAG = "HardwareEncoder";
    private final boolean VERBOSE = false;

    private static final String AUDIO_MIME_TYPE = "audio/mp4a-latm";
    private boolean bEncCreate = false;
    private static final String VIDEO_MIME_TYPE = "video/avc";
    private static final int IFRAME_INTERVAL = 2;

    private boolean mIsGOP0;
    private boolean mIsAllKeyFrame;

    public interface Listener{
        void onFormatChange(MediaFormat newformat);
        void onInputAvaliable(ByteBuffer inData, int[] size, long[] timeUs , int remaining);
        void onGetData(ByteBuffer encodedData, MediaCodec.BufferInfo bufferInfo);
    }

    private Listener mListener;
    private MediaCodec mEncoder;
    private boolean misAudioEnc = false;
    private Surface mInputSurface;

    private static MediaCodecInfo selectCodec(String mimeType) {
        int numCodecs = MediaCodecList.getCodecCount();
        for (int i = 0; i < numCodecs; i++) {
            MediaCodecInfo codecInfo = MediaCodecList.getCodecInfoAt(i);

            if (!codecInfo.isEncoder()) {
                continue;
            }

            String[] types = codecInfo.getSupportedTypes();
            for (int j = 0; j < types.length; j++) {
                if (types[j].equalsIgnoreCase(mimeType)) {
                    return codecInfo;
                }
            }
        }
        return null;
    }

    MediaCodecInfo.CodecProfileLevel findNearestProfileLevel(int profile, int level){
        MediaCodecInfo.CodecProfileLevel retPl = null;
        int mindistance = 65535;
        MediaCodecInfo codecInfo = selectCodec(VIDEO_MIME_TYPE);
        if (codecInfo != null){
            MediaCodecInfo.CodecProfileLevel[] codecProfileLevels = codecInfo.getCapabilitiesForType(VIDEO_MIME_TYPE).profileLevels;
            for (MediaCodecInfo.CodecProfileLevel pl : codecProfileLevels){
                    if (profile == pl.profile){
                        int distance = Math.abs(level - pl.level);
                        if (distance < mindistance){
                            mindistance = distance;
                            retPl = pl;
                    }
                }
            }
        }

        return retPl;
    }

    public HardwareEncoder(@NonNull Listener listener, boolean isAudioEnc){
        mListener = listener;
        misAudioEnc = isAudioEnc;
    }

    /**
     * Returns the encoder's input surface.
     */
    public Surface getInputSurface() {
        return mInputSurface;
    }


    public void startVideoEncoder(int frameRate, int width, int height, int bitRate,boolean isAllKeyFrame) throws IOException {
        mIsAllKeyFrame = isAllKeyFrame;
          Log.d("meeee",  " isK="+mIsAllKeyFrame);
        MediaFormat format = MediaFormat.createVideoFormat(VIDEO_MIME_TYPE, width, height);

        // Set some properties.  Failing to specify some of these can cause the MediaCodec
        // configure() call to throw an unhelpful exception.
        format.setInteger(MediaFormat.KEY_COLOR_FORMAT,
                MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
        format.setInteger(MediaFormat.KEY_BIT_RATE, bitRate);
        format.setInteger(MediaFormat.KEY_FRAME_RATE, frameRate);
        format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, mIsAllKeyFrame? 0 : IFRAME_INTERVAL);

        //TODO: set profile level , bitrate mode
//        MediaCodecInfo.CodecProfileLevel profileLevel = findNearestProfileLevel(MediaCodecInfo.CodecProfileLevel.AVCProfileMain,MediaCodecInfo.CodecProfileLevel.AVCLevel31);
//        if (profileLevel != null){
//            format.setInteger(MediaFormat.KEY_PROFILE, profileLevel.profile);
//            format.setInteger(MediaFormat.KEY_LEVEL, profileLevel.level);
//        }
//        format.setInteger(MediaFormat.KEY_BITRATE_MODE,MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR);

        if (VERBOSE) Log.d(TAG, "format: " + format);

        // Create a MediaCodec encoder, and configure it with our format.  Get a Surface
        // we can use for input and wrap it with a class that handles the EGL work.
        try {
            mEncoder = MediaCodec.createEncoderByType(VIDEO_MIME_TYPE);
            mEncoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mInputSurface = mEncoder.createInputSurface();
            mEncoder.start();
        } catch (Exception e) {
            Log.e(TAG, "startVideoEncoder failed " + e);
        }
        bEncCreate = true;
    }
    @TargetApi(19)
    public void requireKeyFrame() {
//        if (!misAudioEnc /*&& !mIsGOP0*/ && mIsAllKeyFrame) {
//            Bundle param = new Bundle();
//            param.putInt(MediaCodec.PARAMETER_KEY_REQUEST_SYNC_FRAME, 0);
//            try {
//                mEncoder.setParameters(param);
//                Log.v("meeee", "e.re" );
//            } catch (Exception e) {
//            }
//        }
    }

    //for Audio
    public void startAudioEncoder(int sampleRate, int channelCount, int bitRate) throws IOException {
        MediaFormat format = MediaFormat.createAudioFormat(AUDIO_MIME_TYPE, sampleRate, channelCount);
        format.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
        format.setInteger(MediaFormat.KEY_BIT_RATE, bitRate);
        format.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 16384);
        try {
            mEncoder = MediaCodec.createEncoderByType(AUDIO_MIME_TYPE);
            mEncoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mEncoder.start();
        } catch (Exception e){
            Log.e(TAG, "startAudioEncoder failed "+e);
        }
//        mAudioTimestampUs = 0;
//        bytesPerSecond = sampleRate * channelCount * 2;
        misAudioEnc = true;
        bEncCreate = true;
    }

    public void drainEncoder(boolean endOfStream) {
        final int TIMEOUT_USEC = 10000;
//        if (VERBOSE) Log.d(TAG, "drainEncoder(" + endOfStream + ")");
        try {
            if (endOfStream) {
                if (VERBOSE) Log.d(TAG, "sending EOS to encoder");
                mEncoder.signalEndOfInputStream();
            }

            ByteBuffer[] encoderOutputBuffers = mEncoder.getOutputBuffers();
            while (true) {
                MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
                int encoderStatus = mEncoder.dequeueOutputBuffer(bufferInfo, 0);
                if (encoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                    // no output available yet
                    if (!endOfStream) {
                        break;      // out of while
                    } else {
                        if (VERBOSE) Log.d(TAG, "no output available, spinning to await EOS");
                    }
                } else if (encoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                    // not expected for an encoder
                    encoderOutputBuffers = mEncoder.getOutputBuffers();
                } else if (encoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    // should happen before receiving buffers, and should only happen once

                    MediaFormat newFormat = mEncoder.getOutputFormat();
                    Log.d(TAG, "encoder output format changed: " + newFormat);

                    mListener.onFormatChange(newFormat);
                } else if (encoderStatus < 0) {
                    Log.w(TAG, "unexpected result from encoder.dequeueOutputBuffer: " +
                            encoderStatus);
                    // let's ignore it
                } else {

                    ByteBuffer encodedData = encoderOutputBuffers[encoderStatus];
                    if (encodedData == null) {
                        throw new RuntimeException("encoderOutputBuffer " + encoderStatus +
                                " was null");
                    }


                    if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_CODEC_CONFIG) != 0) {
                        // The codec config data was pulled out and fed to the muxer when we got
                        // the INFO_OUTPUT_FORMAT_CHANGED status.  Ignore it.
                        if (VERBOSE) Log.d(TAG, "ignoring BUFFER_FLAG_CODEC_CONFIG");
                        bufferInfo.size = 0;
                    }

                    if (bufferInfo.size > 0) {

                        // adjust the ByteBuffer values to match BufferInfo (not needed?)
                        encodedData.position(bufferInfo.offset);
                        encodedData.limit(bufferInfo.offset + bufferInfo.size);
                        if (VERBOSE)
                            Log.d(TAG, "sent " + bufferInfo.size + " bytes to muxer, ts=" + bufferInfo.presentationTimeUs +
                                    "sync frame " + ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_SYNC_FRAME) != 0));
                        if (bufferInfo.presentationTimeUs != 0) {
                            mListener.onGetData(encodedData, bufferInfo);
                        }
                    }

                    mEncoder.releaseOutputBuffer(encoderStatus, false);

                    if ((bufferInfo.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        if (!endOfStream) {
                            Log.w(TAG, "reached end of stream unexpectedly");
                        } else {
                            if (VERBOSE) Log.d(TAG, "end of stream reached");
                        }
                        break;      // out of while
                    }
                }
            }
        } catch (Exception e) {
            Log.e(TAG,"drainEncoder failed "+e);
        }
    }

    public boolean encodeData(ByteBuffer buf, int sizeInBytes, long timestampUs){

        if(mEncoder == null)
            throw new RuntimeException("Audio Encoder not create");
        try {
            ByteBuffer[] buffers = mEncoder.getInputBuffers();
//            do {
            int bufferIndex = mEncoder.dequeueInputBuffer(10000);
//            }while (bufferIndex == MediaCodec.INFO_TRY_AGAIN_LATER && bThreadRuning);

            if (bufferIndex < 0)
                return false;

            ByteBuffer inputBuffer = buffers[bufferIndex];
            int remaining = inputBuffer.remaining();

            if (remaining < sizeInBytes) {
                Log.e(TAG, "AudioEncoderCore remaining < sizeInBytes");
                return false;
            }

            inputBuffer.put(buf);

//        if(ENABLE_DEBUG)
            //Log.e(TAG, "VIDEO:" + remaining + ", " + sizeInBytes + " , " + timestampUs);
            mEncoder.queueInputBuffer(bufferIndex, 0, sizeInBytes, timestampUs, 0);
        } catch (Exception e){
            Log.e(TAG, "encodeData failed "+e);
        }
        return true;
    }

    /**
     * Releases encoder resources.
     */
    public void release() {
        if (VERBOSE) Log.d(TAG, "releasing encoder objects");
        try {
            if (mEncoder != null) {
                mEncoder.stop();
                mEncoder.release();
                mEncoder = null;
            }
        } catch (Exception e) {
            Log.e(TAG, "release failed " + e);
        }
    }
}
