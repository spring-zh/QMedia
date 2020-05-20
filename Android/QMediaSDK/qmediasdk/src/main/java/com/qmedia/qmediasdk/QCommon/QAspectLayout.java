package com.qmedia.qmediasdk.QCommon;

import android.content.Context;
import android.support.annotation.AttrRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.annotation.StyleRes;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.FrameLayout;

/**
 * Created by spring on 2017/5/8.
 */

public class QAspectLayout extends FrameLayout {
    private static final String TAG = "GLERView";
    private static final boolean VERBOSE = false;
    private double mTargetAspect = -1.0;        // initially use default window size


    public QAspectLayout(@NonNull Context context) {
        super(context);
    }

    public QAspectLayout(@NonNull Context context, @Nullable AttributeSet attrs) {
        super(context, attrs);
    }

    public QAspectLayout(@NonNull Context context, @Nullable AttributeSet attrs, @AttrRes int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        Log.d(TAG, "onMeasure target=" + mTargetAspect +
                " width=[" + MeasureSpec.toString(widthMeasureSpec) +
                "] height=[" + MeasureSpec.toString(heightMeasureSpec) + "]");

        // Target aspect ratio will be < 0 if it hasn't been set yet.  In that case,
        // we just use whatever we've been handed.
        if (mTargetAspect > 0) {
            int initialWidth = MeasureSpec.getSize(widthMeasureSpec);
            int initialHeight = MeasureSpec.getSize(heightMeasureSpec);

            // factor the padding out
            int horizPadding = getPaddingLeft() + getPaddingRight();
            int vertPadding = getPaddingTop() + getPaddingBottom();
            initialWidth -= horizPadding;
            initialHeight -= vertPadding;

            double viewAspectRatio = (double) initialWidth / initialHeight;
            double aspectDiff = mTargetAspect / viewAspectRatio - 1;

            if (Math.abs(aspectDiff) < 0.01) {
                // We're very close already.  We don't want to risk switching from e.g. non-scaled
                // 1280x720 to scaled 1280x719 because of some floating-point round-off error,
                // so if we're really close just leave it alone.
                Log.d(TAG, "aspect ratio is good (target=" + mTargetAspect +
                        ", view=" + initialWidth + "x" + initialHeight + ")");
            } else {
                if (aspectDiff > 0) {
                    // limited by narrow width; restrict height
                    initialHeight = (int) (initialWidth / mTargetAspect);
                } else {
                    // limited by short height; restrict width
                    initialWidth = (int) (initialHeight * mTargetAspect);
                }
                Log.d(TAG, "new size=" + initialWidth + "x" + initialHeight + " + padding " +
                        horizPadding + "x" + vertPadding);
                initialWidth += horizPadding;
                initialHeight += vertPadding;
                widthMeasureSpec = MeasureSpec.makeMeasureSpec(initialWidth, MeasureSpec.EXACTLY);
                heightMeasureSpec = MeasureSpec.makeMeasureSpec(initialHeight, MeasureSpec.EXACTLY);
            }
        }

        //Log.d(TAG, "set width=[" + MeasureSpec.toString(widthMeasureSpec) +
        //        "] height=[" + View.MeasureSpec.toString(heightMeasureSpec) + "]");
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
    }
}
