// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_segment.djinni

package com.qmedia.editor.generated;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import java.util.ArrayList;

public class MediaStreamInfo {

    public static final int MEDIA_UNKNOW = 0;

    public static final int MEDIA_VIDEO = 1;

    public static final int MEDIA_AUDIO = 2;


    /*package*/ final int mMediaType;

    /*package*/ final long mDuration;

    /*package*/ final ArrayList<Long> mGops;

    /*package*/ final AudioDescription mAudioDesc;

    /*package*/ final VideoDescription mVideoDesc;

    public MediaStreamInfo(
            int mediaType,
            long duration,
            @NonNull ArrayList<Long> gops,
            @Nullable AudioDescription audioDesc,
            @Nullable VideoDescription videoDesc) {
        this.mMediaType = mediaType;
        this.mDuration = duration;
        this.mGops = gops;
        this.mAudioDesc = audioDesc;
        this.mVideoDesc = videoDesc;
    }

    public int getMediaType() {
        return mMediaType;
    }

    public long getDuration() {
        return mDuration;
    }

    @NonNull
    public ArrayList<Long> getGops() {
        return mGops;
    }

    @Nullable
    public AudioDescription getAudioDesc() {
        return mAudioDesc;
    }

    @Nullable
    public VideoDescription getVideoDesc() {
        return mVideoDesc;
    }


    @Override
    public String toString() {
        return "MediaStreamInfo{" +
                "mMediaType=" + mMediaType +
                "," + "mDuration=" + mDuration +
                "," + "mGops=" + mGops +
                "," + "mAudioDesc=" + mAudioDesc +
                "," + "mVideoDesc=" + mVideoDesc +
        "}";
    }

}
