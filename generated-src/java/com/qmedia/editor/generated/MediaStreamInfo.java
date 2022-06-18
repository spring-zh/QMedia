// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_comm.djinni

package com.qmedia.editor.generated;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import java.util.ArrayList;

public class MediaStreamInfo {

    /** media type */
    public static final int MEDIA_UNKNOW = 0;

    public static final int MEDIA_VIDEO = 1;

    public static final int MEDIA_AUDIO = 2;

    /** codec type */
    public static final int CODEC_UNKNOW = 0;

    public static final int CODEC_H264 = 1;

    public static final int CODEC_H265 = 2;

    public static final int CODEC_MPEG4 = 3;

    public static final int CODEC_VP8 = 4;

    public static final int CODEC_VP9 = 5;

    public static final int CODEC_MP3 = 11;

    public static final int CODEC_AAC = 12;

    public static final int CODEC_OPUS = 11;

    public static final int CODEC_PCMA = 13;

    public static final int CODEC_PCMU = 14;


    /*package*/ final int mMediaType;

    /*package*/ final int mCodecType;

    /*package*/ final long mDuration;

    /*package*/ final long mBitrate;

    /*package*/ final int mStreamId;

    /*package*/ final ArrayList<Long> mGops;

    /*package*/ final int mTimebaseNum;

    /*package*/ final int mTimebaseDen;

    /*package*/ final AudioDescription mADesc;

    /*package*/ final VideoDescription mVDesc;

    public MediaStreamInfo(
            int mediaType,
            int codecType,
            long duration,
            long bitrate,
            int streamId,
            @NonNull ArrayList<Long> gops,
            int timebaseNum,
            int timebaseDen,
            @NonNull AudioDescription aDesc,
            @NonNull VideoDescription vDesc) {
        this.mMediaType = mediaType;
        this.mCodecType = codecType;
        this.mDuration = duration;
        this.mBitrate = bitrate;
        this.mStreamId = streamId;
        this.mGops = gops;
        this.mTimebaseNum = timebaseNum;
        this.mTimebaseDen = timebaseDen;
        this.mADesc = aDesc;
        this.mVDesc = vDesc;
    }

    public int getMediaType() {
        return mMediaType;
    }

    public int getCodecType() {
        return mCodecType;
    }

    public long getDuration() {
        return mDuration;
    }

    public long getBitrate() {
        return mBitrate;
    }

    public int getStreamId() {
        return mStreamId;
    }

    @NonNull
    public ArrayList<Long> getGops() {
        return mGops;
    }

    /** timebase */
    public int getTimebaseNum() {
        return mTimebaseNum;
    }

    public int getTimebaseDen() {
        return mTimebaseDen;
    }

    @NonNull
    public AudioDescription getADesc() {
        return mADesc;
    }

    @NonNull
    public VideoDescription getVDesc() {
        return mVDesc;
    }


    @Override
    public String toString() {
        return "MediaStreamInfo{" +
                "mMediaType=" + mMediaType +
                "," + "mCodecType=" + mCodecType +
                "," + "mDuration=" + mDuration +
                "," + "mBitrate=" + mBitrate +
                "," + "mStreamId=" + mStreamId +
                "," + "mGops=" + mGops +
                "," + "mTimebaseNum=" + mTimebaseNum +
                "," + "mTimebaseDen=" + mTimebaseDen +
                "," + "mADesc=" + mADesc +
                "," + "mVDesc=" + mVDesc +
        "}";
    }

}
