// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from animator.djinni

package com.qmedia.editor.generated;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public class Animator {

    /** property */
    @NonNull
    public static final String POSITIONXYZ = "positionxyz";

    @NonNull
    public static final String ROTATION = "rotation";

    @NonNull
    public static final String ROTATIONXYZ = "rotationxyz";

    @NonNull
    public static final String SCALEX = "scalex";

    @NonNull
    public static final String SCALEY = "scaley";

    @NonNull
    public static final String SCALEZ = "scalez";

    @NonNull
    public static final String SCALEXY = "scalexy";

    @NonNull
    public static final String SCALEXYZ = "scalexyz";

    @NonNull
    public static final String CONTENTSIZEW = "contentsizew";

    @NonNull
    public static final String CONTENTSIZEH = "contentsizeh";

    @NonNull
    public static final String CONTENTSIZE = "contentsize";

    @NonNull
    public static final String ALPHA = "alpha";

    @NonNull
    public static final String MIXCOLOR = "mixcolor";

    @NonNull
    public static final String CROP = "crop";


    /*package*/ final String mName;

    /*package*/ final String mProperty;

    /*package*/ final MediaRange mTimerang;

    /*package*/ final Vec4f mStartpoint;

    /*package*/ final Vec4f mEndpoint;

    /*package*/ final EaseFunction mEasefunc;

    /*package*/ final boolean mRepleat;

    public Animator(
            @NonNull String name,
            @NonNull String property,
            @NonNull MediaRange timerang,
            @NonNull Vec4f startpoint,
            @NonNull Vec4f endpoint,
            @NonNull EaseFunction easefunc,
            boolean repleat) {
        this.mName = name;
        this.mProperty = property;
        this.mTimerang = timerang;
        this.mStartpoint = startpoint;
        this.mEndpoint = endpoint;
        this.mEasefunc = easefunc;
        this.mRepleat = repleat;
    }

    @NonNull
    public String getName() {
        return mName;
    }

    @NonNull
    public String getProperty() {
        return mProperty;
    }

    @NonNull
    public MediaRange getTimerang() {
        return mTimerang;
    }

    @NonNull
    public Vec4f getStartpoint() {
        return mStartpoint;
    }

    @NonNull
    public Vec4f getEndpoint() {
        return mEndpoint;
    }

    @NonNull
    public EaseFunction getEasefunc() {
        return mEasefunc;
    }

    public boolean getRepleat() {
        return mRepleat;
    }


    @Override
    public String toString() {
        return "Animator{" +
                "mName=" + mName +
                "," + "mProperty=" + mProperty +
                "," + "mTimerang=" + mTimerang +
                "," + "mStartpoint=" + mStartpoint +
                "," + "mEndpoint=" + mEndpoint +
                "," + "mEasefunc=" + mEasefunc +
                "," + "mRepleat=" + mRepleat +
        "}";
    }

}