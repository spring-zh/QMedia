package com.qmedia.qmediasdk.sample.sample.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.drawable.ColorDrawable;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.PopupWindow;
import android.widget.TextView;

import com.qmedia.qmediasdk.sample.sample.utils.SystemUtil;
import com.qmedia.qmediasdk.sample.R;


/**
 * Created by AA on 2016/10/13.
 */

public class MediaClipBar extends View {

    private static final String TAG = "meeeeVideoClipBar";
    private Context mContext;
    /**
     * SeekBar数值文字颜色
     */
    private int mTextColor;

    /**
     * SeekBar数值文字大小
     */
    private float mTextSize;

    /**
     * SeekBar数值文字内容
     */
    private String mTimeStampText = "";

    /**
     * SeekBar数值文字画笔
     */
    private Paint mTextPaint;


    private boolean mIsPressed = false;
    /**
     * thumb
     * */
    private static final int THUMB_PRESSED_COLOR = 0XFFA098C6 ;
    private static final int THUMB_NORMAL_COLOR =  0XFFBDEAC0;
    private Paint mThumbPaint = new Paint();

    private String mVideoClipName = "";


    private float mProgressLeftX = -1;
    private float mProgressRight = -1;
    private float mProgressW = -1;
    private float mTouchedX = -1;
    private float mViewWidth = -1;
    private float mViewHeight = -1;



    //time stamp popupwindow
    private PopupWindow mPopupW ;
    private TextView mTextView;

//    private MediaObject mMediaObject;
    private int mClipType = -1;

    public MediaClipBar(Context context) {
        this(context, null);
    }

    public MediaClipBar(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public MediaClipBar(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        mContext = context;
        mTextColor = getResources().getColor(R.color.md_white_1000);
        mTextSize = getResources().getDimension(R.dimen.badge_text_size);
setClickable(true);
        //设置画笔
        mTextPaint = new Paint();
        mTextPaint.setAntiAlias(true);
        mTextPaint.setColor(mTextColor);
        mTextPaint.setTextSize(mTextSize);
        mThumbPaint.setColor(Color.BLACK);
        initPopupWindow();


    }

    private void initPopupWindow(){
        View v  = LayoutInflater.from(mContext).inflate(R.layout.popup_window_layout, null);
        mTextView = (TextView)v.findViewById(R.id.popupTime);
        mPopupW = new PopupWindow(v, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, false);
        //popupWindow.setAnimationStyle(R.style.AnimationFade);
        mPopupW.setBackgroundDrawable(new ColorDrawable(0));
    }


    private int mMaxDuration = -1;
    private int mClipDuration = -1;


    public void setMax(int maxMs){
        mMaxDuration = maxMs;
        mProgressW = mViewWidth*mClipDuration/ mMaxDuration;
    }

    private void setClipDuration(int ms){
        mClipDuration = ms;
        mProgressW = mViewWidth*mClipDuration/ mMaxDuration;
    }

//    public void setMediaObject(MediaObject mediaObject) {
//        mMediaObject = mediaObject;
//        mClipType = mediaObject.getDataType();
//        mClipDuration = mMediaObject.getDuration();
//        String fullName = mMediaObject.getPath();
//
//        mVideoClipName = fullName.substring(fullName.lastIndexOf("/") + 1);
//        int len = mVideoClipName.length();
//        if (len > 30) {
//            mVideoClipName = mVideoClipName.substring(len - 30);
//        }
//
//    }

    public int getProgressMs(){
        return (int)(mProgressLeftX/mViewWidth*mMaxDuration);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        mViewWidth = w;
        mViewHeight = h;
        mProgressW = mViewWidth*mClipDuration/ mMaxDuration;
    }

    @Override
    protected  void onDraw(Canvas canvas) {

        super.onDraw(canvas);
        //some media clip's duration may be very shot related to the total duration of the composed media,
        // so we have to limit the min width of the clip(mProgressW) to 1/20 of mViewWidth,or the clip's width can be fairly small that we can not see it
        mProgressW = 1.0f*mClipDuration/mMaxDuration*mViewWidth;

        if (mProgressW < mViewWidth / 20) {
            mProgressW = mViewWidth / 20;
        }

        if (mProgressLeftX == -1&&mProgressRight == -1){
            mProgressLeftX = 0;
            mProgressRight = mProgressW;
        }

        //更新时间戳
        mTimeStampText =   SystemUtil.getForamtedTime((int)(mProgressLeftX/mViewWidth* mMaxDuration));

        mTextView.setText(mTimeStampText);

        if (mIsPressed){
            mThumbPaint.setColor(THUMB_PRESSED_COLOR);
        }else{
            mThumbPaint.setColor(THUMB_NORMAL_COLOR);
        }

        canvas.drawRect(mProgressLeftX, 0 , mProgressRight, mViewHeight, mThumbPaint);

        //绘制文件名字符
        Paint.FontMetrics fm = mTextPaint.getFontMetrics();
         float fileTextBaseLine = (mViewHeight) / 2 - (fm.descent + fm.ascent) / 2;
        canvas.drawText(mVideoClipName, mProgressLeftX+5 , fileTextBaseLine, mTextPaint);

    }

    private boolean mIsL2R = false;
    private float mLastTouched = -10000;
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        mTouchedX = event.getX();
        if (mTouchedX > mLastTouched){
            mIsL2R = true;
        }else{
            mIsL2R = false;
        }
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                mIsPressed = true;
                mPopupW.showAsDropDown(this,(int)mProgressLeftX,-((int)mViewHeight +mTextView.getHeight()));
                break;
            case MotionEvent.ACTION_MOVE:
                cacluateCord();
                mPopupW.update(this, (int)mProgressLeftX, -((int)mViewHeight +mTextView.getHeight()), -1, -1);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                mIsPressed = false;
                mPopupW.dismiss();
//                mMediaObject.getTimeLayout().fileStartTime = getProgressMs();
        }
        mLastTouched  = mTouchedX;
        invalidate();
        return super.onTouchEvent(event);
    }

    private void cacluateCord(){
        if (mProgressRight<=mProgressW*0.75){
            mProgressLeftX +=  mTouchedX-mLastTouched;
            mProgressRight +=  mTouchedX-mLastTouched;
        }else if(mProgressLeftX>=mViewWidth-mProgressW*0.75){
            mProgressLeftX +=  mTouchedX-mLastTouched;
            mProgressRight +=  mTouchedX-mLastTouched;
        }else {
            mProgressLeftX = mTouchedX - mProgressW / 2;
            mProgressRight = mTouchedX + mProgressW / 2;
        }



    }


    public void setProgress(int ms){
        mProgressLeftX = ms/mMaxDuration*mViewWidth;
    }
}
