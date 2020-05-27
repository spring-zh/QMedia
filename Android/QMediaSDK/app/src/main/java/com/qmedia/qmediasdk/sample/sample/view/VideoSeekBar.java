/**
 * Created by yuelei on 2017/7/3.
 */

package com.qmedia.qmediasdk.sample.sample.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
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

import com.qmedia.qmediasdk.sample.R;

/**
 * 视频(缩略图、裁剪)进度滑动条
 */
public class VideoSeekBar extends View {


    private final String TAG = "meeeeVideoSeekBar";
    private Context mContext;

    /**
     * 滑动的图片(左右两个)
     */
    private Bitmap mLeftBitmap, mRightBitmap;
    /**
     * 当前View 宽度
     */
    private int mViewWidth;
    /**
     * 当前View 高度
     */
    private int mViewHeight;
    /**
     * 最小可裁剪时长时间 单位毫秒
     */
    private long mMinTrimZoneMs = 0;

    /**
     * 当前进度值
     */
    private int mCurrentProgress = 0;
    /**
     * 最大进度值
     */
    private int mMaxProgress = 100;

    /**
     * 屏幕上坐标转换时间 - X轴 横
     */
    private float xTime = -1f;

    /**
     * 是否显示sliderBar
     */
    private boolean mShowSliderBar = false;
    /**
     * 是否绘制播放进度条
     */
    private boolean mShowProgressLine = false;
    /**
     * 是否绘制播放进度背景
     */
    private boolean mShowProgressBG = false;

    private boolean mSelectedZoneDragEnable = false;

    private boolean mIsSeekToEnd = false;

    private float mLastXpoint = -0xff;
    private boolean mIsSlideL2R;

    // -- 画笔 --
    /**
     * 绘制缩略图画笔
     */
    private Paint mThumbPaint = new Paint();
    private Paint mProgressLinePaint = new Paint();
    private Paint mProgressBgPaint = new Paint();
    private Paint mUnselectedZoneBgPaint = new Paint();

    private int mBlackGoundColor = 0xFFE6E0EC;

    private int mPoinrtWidth = -1;

    /**
     * A callback that notifies clients when the progress level has been
     * changed. This includes changes that were initiated by the user through a
     * touch gesture or arrow key/trackball as well as changes that were initiated
     * programmatically.
     */
    public interface OnSeekBarChangeListener {

        /**
         * Notification that the progress level has changed. Clients can use the fromUser parameter
         * to distinguish user-initiated changes from those that occurred programmatically.
         *
         * @param seekBar  The SeekBar whose progress has changed
         * @param progress The current progress level. This will be in the range 0..max where max
         *                 was set by {@link VideoSeekBar#setMax(int)}. (The default value for max is 100.)
         * @param which    which part is touched
         */
        void onProgressChanged(VideoSeekBar seekBar, int progress, int which);

        /**
         * Notification that the user has started a touch gesture. Clients may want to use this
         * to disable advancing the seekbar.
         *
         * @param seekBar The SeekBar in which the touch gesture began
         * @param which   which part is touched
         */
        void onStartTrackingTouch(VideoSeekBar seekBar, int which);

        /**
         * Notification that the user has finished a touch gesture. Clients may want to use this
         * to re-enable advancing the seekbar.
         *
         * @param seekBar The SeekBar in which the touch gesture began
         * @param which   which part is touched
         */
        void onStopTrackingTouch(VideoSeekBar seekBar, int which);

        /**
         * Notification that the user has finished a touch gesture. Clients may want to use this
         * to re-enable advancing the seekbar.
         *
         * @param seekBar The SeekBar in which the touch gesture began
         * @param which   which part is touched
         */
        //void onProgressSeekToEnd(VideoSeekBar seekBar, int which);
    }


    private OnSeekBarChangeListener mOnSeekBarChangeListener;


    //seek pointer popupwindow
    private PopupWindow mPopupW ;
    private void initPopupWindow(){
        View v  = LayoutInflater.from(mContext).inflate(R.layout.popup_pointer_layout, null);
        mPopupW = new PopupWindow(v, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, false);
        //popupWindow.setAnimationStyle(R.style.AnimationFade);
        mPopupW.setBackgroundDrawable(new ColorDrawable(0));
        mPopupW.setFocusable(false);
        mPopupW.setTouchable(false);
    }

    public void setOnSeekBarChangeListener(OnSeekBarChangeListener l) {
        mOnSeekBarChangeListener = l;
    }

    public VideoSeekBar(Context context) {
        super(context);
        init(context);
    }

    public VideoSeekBar(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    public VideoSeekBar(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init(context);
    }

    private void onFirstShow() {
        if (mProgressX == -1 || mLeftSX == -1 || mRightSX == -1) {
            mLeftSX = 0;
            mProgressX = mSliderIgWidth;
            mRightSX = mViewWidth - mSliderIgWidth;
        }
    }

    //只负责根据坐标画，不计算坐标
    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        //首先绘制大背景
        canvas.drawColor(mBlackGoundColor);
        onFirstShow();
        //
        if (mShowSliderBar) {
            // 绘制左边边滑动的X轴位置
            canvas.drawBitmap(mLeftBitmap, mLeftSX, 0, mThumbPaint);
            // 绘制右边滑动的X轴位置
            canvas.drawBitmap(mRightBitmap, mRightSX, 0, mThumbPaint);
            // === 绘制左边拖动阴影图层 ===
            canvas.drawRect(0, 0, mLeftSX, mViewHeight, mUnselectedZoneBgPaint);
            // === 绘制右边拖动阴影图层 ===
            canvas.drawRect(mRightSX + mSliderIgWidth, 0, mViewWidth, mViewHeight, mUnselectedZoneBgPaint);
            //Log.v("meeee", "绘制裁剪框L@" + mLeftSX + ",r@" + mRightSX);
        }
        // 是否绘制播放进度条及进度背景
        if (mShowProgressLine) {

            //canvas.drawRect(mProgressX, 0, mProgressX + mProgressW, mViewHeight, mProgressLinePaint);
            if (mShowProgressBG) {
                canvas.drawRect(mLeftSX + mSliderIgWidth, 0, mProgressX+mProgressW, mViewHeight, mProgressBgPaint);
            }

        }

        if(mPopupW.isShowing()){
            mPopupW.update(this, (int)(mProgressX-mProgressW-mPoinrtWidth/2), -mViewHeight, -1, -1);
        } else {
            mPopupW.showAsDropDown(this,(int)(mProgressX-mProgressW-mPoinrtWidth/2),-mViewHeight);
        }

    }


    // =====================

    /**
     * 左边滑动"["的X轴
     */
    private float mLeftSX = -1f;
    /**
     * 右边滑动"]"的X轴
     */
    private float mRightSX = -1f;
    /**
     * 滑动的图片"["或"]"宽度
     */
    private int mSliderIgWidth = 0;
    /**
     * 进度拖动按钮的X轴
     */
    private float mProgressX = -1;
    private float mProgressW = 0;

    /**
     * min pix from left to right slider
     */
    private float mMinL2RPixs = 0;


    /**
     * 滑动的View类型
     */
    private int mTouchViewMode = -1;
    /**
     * 滑动左边的View
     */
    public static final int TOUCH_LEFT_VIEW = 1;
    /**
     * 滑动右边的View
     */
    public static final int TOUCH_RIGHT_VIEW = 2;
    /**
     * 滑动左右两边中间空白部分
     */
    public static final int TOUCH_MIDST_VIEW = 3;
    /**
     * 滑动左右两边中间空白部分
     */
    public static final int TOUCH_SEEK_BUTTON = 4;

    /**
     * touch action
     */
    private int mTouchAction = -1;

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        super.onTouchEvent(event);
        // 滑动中的X轴位置
        float xMove = event.getX();
        if (xMove > mLastXpoint) {
            mIsSlideL2R = true;
        } else {
            mIsSlideL2R = false;
        }
        //Log.v(TAG, "l2r ? =" + mIsSlideL2R);
        mLastXpoint = xMove;
        mTouchAction = event.getAction();
        switch (mTouchAction) {
            case MotionEvent.ACTION_DOWN: // 按下时
                //check which node is pressed
                if (xMove >= (mProgressX - mProgressW / 2) && xMove <= (mProgressX + mProgressW * 1.5)) {
                    mTouchViewMode = TOUCH_SEEK_BUTTON;
                    cacluateCord(xMove);
                } else if (xMove >= (mLeftSX - mSliderIgWidth / 2) && xMove <= (mLeftSX + ((float) mSliderIgWidth) * 1.5)) {
                    mTouchViewMode = TOUCH_LEFT_VIEW;
                    cacluateCord(xMove);
                } else if (xMove >= (mRightSX - mSliderIgWidth / 2) && xMove <= (mRightSX + ((float) mSliderIgWidth) * 1.5)) {
                    mTouchViewMode = TOUCH_RIGHT_VIEW;
                    cacluateCord(xMove);
                } else {
                    mTouchViewMode = -1;
                    //没有点击seekbar或滑动条，不消费事件
                    return false;
                }
                if (mOnSeekBarChangeListener != null) {
                    mOnSeekBarChangeListener.onStartTrackingTouch(this, mTouchViewMode);
                }
                //Log.v(TAG, "X = " + xMove + " Left=" + mLeftSX + " Right=" + mRightSX + " mProgressX=" + mProgressX + ",mTouchViewMode=" + mTouchViewMode);
                break;
            case MotionEvent.ACTION_MOVE: // 滑动中
                // 计算滑动距离
                cacluateCord(xMove);

                if (mOnSeekBarChangeListener != null) {
                    int progress = -1;
                    switch (mTouchViewMode) {
                        case TOUCH_LEFT_VIEW:
                            progress = getStartTime();
                            break;
                        case TOUCH_RIGHT_VIEW:
                            progress = getEndTime();
                            break;
                        case TOUCH_SEEK_BUTTON:
                            progress = getProgress();
                            break;

                    }
                    mOnSeekBarChangeListener.onProgressChanged(this, progress, mTouchViewMode);

                }
                break;
            case MotionEvent.ACTION_UP: // 抬起时
                mTouchViewMode = -1;
                if (mOnSeekBarChangeListener != null) {
                    mOnSeekBarChangeListener.onStopTrackingTouch(this, mTouchViewMode);
                }
                break;
        }


        return true;
    }


    /**
     * 计算滑动
     *
     * @param xMove 滑动的X轴
     */
    private void cacluateCord(float xMove) {
        // 计算右边边距值
        // 如果都不属于滑动,则不处理
        //Log.v(TAG,"mTouchViewMode = "+mTouchViewMode);
        if (!(mTouchViewMode == TOUCH_LEFT_VIEW || mTouchViewMode == TOUCH_RIGHT_VIEW
                || mTouchViewMode == TOUCH_MIDST_VIEW || mTouchViewMode == TOUCH_SEEK_BUTTON)) {
            return;
        }

        if (xTime <= 0) {
            // 视频总进度 / 宽度 = 每个坐标占用多少毫秒
            xTime = mMaxProgress / mViewWidth;
            mMinL2RPixs = (mMinTrimZoneMs / xTime > mProgressW) ? mMinTrimZoneMs / xTime : mProgressW;
            //Log.v(TAG, "mMinL2RPixs=" + mMinL2RPixs);
        }

        if (mMinL2RPixs <= 0) {
            mMinL2RPixs = mProgressW;
        }

        if (mTouchAction == MotionEvent.ACTION_DOWN) {
            ;//do what?
            if (xMove < 0) {
                xMove = 0;
            }
            if (xMove >= mViewWidth) {
                xMove = mViewWidth;
            }
        } else if (mTouchAction == MotionEvent.ACTION_MOVE) {

            if (mTouchViewMode == TOUCH_LEFT_VIEW) { // 属于滑动左边图片

                if (xMove > mRightSX - mMinL2RPixs - mSliderIgWidth) {
                    mLeftSX = mRightSX - mMinL2RPixs - mSliderIgWidth;
                } else if (xMove > 0 && mLeftSX <= mRightSX - mMinL2RPixs - mSliderIgWidth) {
                    mLeftSX = xMove;
                } else {
                    mLeftSX = 0;
                }

                //Log.v(TAG, "mLeftSX = " + mLeftSX + "     mRightSX=" + mRightSX);
                if (mProgressX < mLeftSX + mSliderIgWidth) {
                    mProgressX = mLeftSX + mSliderIgWidth;
                } else if (mProgressX > mRightSX - mProgressW) {
                    mProgressX = mRightSX - mProgressW;
                }
            } else if (mTouchViewMode == TOUCH_RIGHT_VIEW) { // 属于滑动右边图片
                if (xMove >= mViewWidth - mSliderIgWidth) { // 滑动到边缘则直接设置边缘
                    mRightSX = mViewWidth - mSliderIgWidth;
                } else if (xMove > mMinL2RPixs + mLeftSX + mSliderIgWidth && xMove < mViewWidth - mSliderIgWidth) {
                    mRightSX = xMove;
                } else { // 如果触碰了
                    mRightSX = mMinL2RPixs + mLeftSX + mSliderIgWidth;
                }

                if (mProgressX < mLeftSX + mSliderIgWidth) {
                    mProgressX = mLeftSX + mSliderIgWidth;
                } else if (mProgressX > mRightSX - mProgressW) {
                    mProgressX = mRightSX - mProgressW;
                }
                //Log.v(TAG, "mLeftSX = " + mLeftSX + "     mRightSX=" + mRightSX);
            } else if (mTouchViewMode == TOUCH_SEEK_BUTTON) {
                if (xMove < mLeftSX + mSliderIgWidth) {
                    mProgressX = mLeftSX + mSliderIgWidth;
                } else if (xMove > mRightSX - mProgressW) {
                    mProgressX = mRightSX - mProgressW;
                } else {
                    mProgressX = xMove;
                }
                //Log.v(TAG,"seekbutton@ mProgressX="+mProgressX+ "   mLeftSX="+mLeftSX+"   mRightSX="+mRightSX);
            }

        } else if (mTouchAction == MotionEvent.ACTION_UP) {
            mTouchViewMode = -1;
            mTouchViewMode = -1;
        }
        invalidate();

    }


    /**
     * 初始化操作
     */
    private void init(Context context) {
        mContext = context;
        // 防止不进行绘画 触发onDraw
        setWillNotDraw(false);
        // 获取左右两个滑动的图片
        mLeftBitmap = BitmapFactory.decodeResource(getContext().getResources(), R.drawable.ic_left_sliderbar);
        mRightBitmap = BitmapFactory.decodeResource(getContext().getResources(), R.drawable.ic_right_sliderbar);
        // 先保存滑动图片宽度
        mSliderIgWidth = mLeftBitmap.getWidth();
        mPoinrtWidth =  BitmapFactory.decodeResource(getContext().getResources(), R.drawable.pointer).getWidth();
        mProgressW = 5 * (int) (mContext.getResources().getDisplayMetrics().density + 0.5f);
        // 初始化画笔
        initPaint();
        initPopupWindow();
    }


    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh) {
        super.onSizeChanged(w, h, oldw, oldh);
        mViewWidth = w;
        mViewHeight = h;
        if (mViewWidth != 0) {
            // 视频总进度 / 宽度 = 每个坐标占用多少毫秒
            xTime = mMaxProgress / mViewWidth;
        }
        //计算滑动的边距
        getSliderBitmap(true);
    }

    /**
     * 初始化画笔
     */
    private void initPaint() {
        // 初始化画笔
        mThumbPaint = new Paint(Paint.ANTI_ALIAS_FLAG); // 缩略图
        mProgressLinePaint = new Paint(Paint.ANTI_ALIAS_FLAG); // 播放进度 白色竖直线条
        mProgressLinePaint.setColor(Color.rgb(255, 0, 0)); // 字体颜色 - 白色

        mProgressBgPaint = new Paint(Paint.ANTI_ALIAS_FLAG); // 播放进度背景,半透明(画布遮挡层)
        mProgressBgPaint.setColor(Color.rgb(0, 0, 0)); // 背景进度颜色(画布遮挡层)
        mProgressBgPaint.setAlpha(50); // 画布遮挡层

        mUnselectedZoneBgPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mUnselectedZoneBgPaint.setColor(Color.rgb(0, 0, 0)); // 背景进度颜色(画布遮挡层)
        mUnselectedZoneBgPaint.setAlpha(200); // 画布遮挡层


    }


    /**
     * 获取滑动图片,根据View大小调整大小
     *
     * @param isLeft
     * @return
     */
    private Bitmap getSliderBitmap(boolean isLeft) {
        // 防止高度为0
        if (mViewHeight != 0) {
            // 获取高度进行计算
            int bHeight = mLeftBitmap.getHeight();
            // 判断是否需要缩放,高度不一直则要求缩放
            if (bHeight != mViewHeight) {
                // 获取图片宽度
                int bWidth = mLeftBitmap.getWidth();
                // 计算宽度比例
                bWidth = (int) (((float) mViewHeight / (float) bHeight) * bWidth);
                // 保存缩放比例后的宽度
                mSliderIgWidth = bWidth;
                // 进行比例缩放图片
                mLeftBitmap = Bitmap.createScaledBitmap(mLeftBitmap, bWidth, mViewHeight, true);
                mRightBitmap = Bitmap.createScaledBitmap(mRightBitmap, bWidth, mViewHeight, true);
            }
        }
        return isLeft ? mLeftBitmap : mRightBitmap;
    }


    /**
     * 进行重置
     */
    public void reset() {
        mLeftSX = 0f; // 重置到最左边
        mRightSX = 0f; // 重置到最右边
        mProgressX = 0f; //
    }

    /**
     * 是否允许裁剪(判断是否拖动)
     *
     * @return
     */
    public boolean isTrimVideo() {
        return false;
    }

    /**
     * 获取开始时间(左边X轴转换时间) - 毫秒
     *
     * @return
     */
    public int getStartTime() {
        return (int) ((mLeftSX +mSliderIgWidth)* xTime);
    }

    /**
     * 获取结束时间(右边X轴转换时间) - 毫秒
     *
     * @return
     */
    public int getEndTime() {
        return (int) (mRightSX * xTime);
    }

    public int getProgresserSpaceTime(){
        return (int)(mProgressW*xTime);
    }


    public void setProgress(int curTime) {
        int time = curTime <= 0 ? 0 : curTime;
        if (xTime <= 0) {
            xTime = mMaxProgress / mViewWidth;
        }
        mProgressX = time / xTime;
        //Log.v(TAG,"setProgress@"+mProgressX + "   mLeftSX="+mLeftSX+"    mRightSX="+mRightSX + "   xTime="+xTime+"   time="+time);
        if (mProgressX < mLeftSX + mSliderIgWidth) {
            mProgressX = mLeftSX + mSliderIgWidth;
        } else if (mProgressX > mRightSX - mProgressW) {
            mProgressX = mRightSX - mProgressW;
        }
        invalidate();
    }


    public int getProgress() {
        return (int) (mProgressX * xTime);
    }

    public int getMax() {
        return mMaxProgress;
    }


    public void setMax(int maxDuration) {
        mMaxProgress = maxDuration;
        xTime = mMaxProgress / mViewWidth;
    }


    public void setMinTrimZoneMs(int ms) {
        mMinTrimZoneMs = ms;
    }

    public void showProgressLine(boolean isShowProgressLine) {
        this.mShowProgressLine = isShowProgressLine;
    }

    public void showProgressBG(boolean ishowProgressBG) {
        this.mShowProgressBG = ishowProgressBG;
    }

    public void showSlideBar(boolean showSlideBar) {
        this.mShowSliderBar = showSlideBar;
    }

    public void enableDragSelectedZone(boolean isEnable) {
        mSelectedZoneDragEnable = isEnable;
    }


    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        dismissPopupWindow();
    }

    public void dismissPopupWindow(){
        if (mPopupW.isShowing()) {
            mPopupW.dismiss();
        }
    }

}
