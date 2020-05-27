package com.qmedia.qmediasdk.sample.sample.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;

import com.qmedia.qmediasdk.sample.R;
/**
 * 此控件是整个scrollbar，包含上下两个控制按钮
 * 
 * @Author ZGY
 * @Date:2014-3-27
 * @version
 * @since 欢迎加QQ群88130145进行开发交流
 * 
 */
@SuppressLint("HandlerLeak")
public class ListWithScrollBarView extends LinearLayout implements OnTouchListener {

	private Context mContext;
	private ScrollBar mScrollBar;
	private ListView mListView;
	private Handler mHandler;

	private boolean mListViewOnScrollBarCtrled = false;// listview是否是滚动条控制的滚动
	int mFirstVisibleItem;
	int mVisibleItemCount;
	int mTotalItemCount;
	private boolean mHasLeftItems;// 是否有显示不下的item，防止滚动时scrollbar高度被重置

	// Listener
	private AbsListView.OnScrollListener mOnListViewScrollListener;
	private ScrollBar.OnBarListener mOnBarScrollListener;

	public ListWithScrollBarView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init(context);
	}

	public ListWithScrollBarView(Context context) {
		super(context);
		init(context);
	}

	private void init(Context context) {

		this.mContext = context;
		mHandler = new LoopHandler();
		LayoutInflater inflater = LayoutInflater.from(mContext);
		ViewGroup main = (ViewGroup) inflater.inflate(R.layout.scroll_ctrl_view, null);

		mListView = (ListView) main.findViewById(R.id.list_view);
		ImageView imgUp = (ImageView) main.findViewById(R.id.img_scroll_ctrl_up);
		ImageView imgDown = (ImageView) main.findViewById(R.id.img_scroll_ctrl_down);
		mScrollBar = (ScrollBar) main.findViewById(R.id.scrollbar);

		imgDown.setOnTouchListener(this);
		imgUp.setOnTouchListener(this);

		this.addView(main);

		mListView.setOnScrollListener(new AbsListView.OnScrollListener() {
			@Override
			public void onScrollStateChanged(AbsListView view, int scrollState) {
				if (mOnListViewScrollListener != null) {
					mOnListViewScrollListener.onScrollStateChanged(view, scrollState);
				}
			}

			@Override
			public void onScroll(AbsListView view, int firstVisibleItem, int visibleItemCount, int totalItemCount) {
				mFirstVisibleItem = firstVisibleItem;
				mVisibleItemCount = visibleItemCount;
				mTotalItemCount = totalItemCount;
				// Log.e("", "mFirstVisibleItem =" + mFirstVisibleItem + " mVisibleItemCount=" + mVisibleItemCount + "  mTotalItemCount" + mTotalItemCount);
				if (!mListViewOnScrollBarCtrled) {
					// Log.i("", "onScroll");
					// 仅当listview触摸滑动时才控制滚动条位置
					if ((totalItemCount - visibleItemCount) > 0) {
						Log.i("", "totalItemCount > visibleItemCount");
						if (!mHasLeftItems) {
							// 之前是不满一屏的，现在超过一屏了，则更新滚动条的长度
							mScrollBar.resetScrollBar(false);
						}
						mHasLeftItems = true;
						setScrollBarProgress(firstVisibleItem * mScrollBar.getMaxProgress() / (totalItemCount - visibleItemCount));
					} else {
						// 不满1屏，则滚动条长度铺满
//						if (totalItemCount != 0) {
							if (visibleItemCount <= totalItemCount && !mHasLeftItems) {
								mScrollBar.resetScrollBar(true);
							}
//						}
					}

				} else {
				}

				if (mOnListViewScrollListener != null) {
					mOnListViewScrollListener.onScroll(view, firstVisibleItem, visibleItemCount, totalItemCount);
				}
			}
		});

		mScrollBar.setListener(new ScrollBar.OnBarListener() {

			@Override
			public void onBarProgressChanged(ScrollBar verticalScrollBar, int progress, int maxProgress) {
				// TODO 滚动条位置有变化，控制listview的滚动，此处待优化
				int shouldShowPosition = (mTotalItemCount - mVisibleItemCount) * progress / mScrollBar.getMaxProgress();
				// mListView.smoothScrollToPosition(shouldShowPosition);//平滑滚动到某个位置
				// mListView.smoothScrollToPosition(shouldShowPosition, 1);
				mListView.setSelection(shouldShowPosition);// 直接控制展示到某位置

				if (mOnBarScrollListener != null) {
					mOnBarScrollListener.onBarProgressChanged(verticalScrollBar, progress, maxProgress);
				}
			}

			@Override
			public void onBarControled(boolean underCtrl) {
				mListViewOnScrollBarCtrled = underCtrl;

				if (mOnBarScrollListener != null) {
					mOnBarScrollListener.onBarControled(underCtrl);
				}
			}

		});
	}

	@Override
	public boolean onTouch(View v, MotionEvent event) {
		// 如果按住上下按钮不动，则一直控制scrollbar的滚动

		switch (event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			mListViewOnScrollBarCtrled = true;
			if (v.getId() == R.id.img_scroll_ctrl_up) {
				mHandler.sendEmptyMessage(MSG_UP);
			} else {
				mHandler.sendEmptyMessage(MSG_DOWN);
			}
			break;

		case MotionEvent.ACTION_UP:
			mListViewOnScrollBarCtrled = false;
			mHandler.removeMessages(MSG_UP);
			mHandler.removeMessages(MSG_DOWN);
			break;
		case MotionEvent.ACTION_CANCEL:
			mListViewOnScrollBarCtrled = false;
			mHandler.removeMessages(MSG_UP);
			mHandler.removeMessages(MSG_DOWN);
			break;

		default:
			break;
		}

		return false;
	}

	private boolean upClicked() {
		if (mScrollBar.getNowProgress() > 0) {
			mScrollBar.setProgress(mScrollBar.getNowProgress() - 1, true);
			return true;
		}

		return false;
	}

	private boolean downClicked() {
		if (mScrollBar.getNowProgress() < mScrollBar.getMaxProgress()) {
			mScrollBar.setProgress(mScrollBar.getNowProgress() + 1, true);
			return true;
		}
		return false;
	}

	// /////////////////
	/**
	 * 当上下按钮被按下时，循环控制滚动条滚动
	 */
	private static final int MSG_UP = 0x100;
	private static final int MSG_DOWN = 0x101;
	private static final int DELAY = 10;

	private class LoopHandler extends Handler {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case MSG_UP:
				if (upClicked()) {
					mHandler.sendEmptyMessageDelayed(MSG_UP, DELAY);
				}

				break;
			case MSG_DOWN:
				if (downClicked()) {
					mHandler.sendEmptyMessageDelayed(MSG_DOWN, DELAY);
				}
				break;

			default:
				break;
			}

		}

	}

	// ///////////////////添加公开的接口////////////////////
	/**
	 * 控制滚动条滚动的位置
	 * 
	 * @param progress
	 */
	public void setScrollBarProgress(int progress) {
		mScrollBar.setProgress(progress, false);
	}

	/**
	 * 获得滚动条的位置
	 * 
	 * @param @return 
	 * @author zhuanggy
	 * @date 2014-3-28
	 */
	public int getScrollBarProgress() {
		return mScrollBar.getNowProgress();
	}

	/**
	 * 设置滚动条滚动区域的最大值
	 * 
	 * @param @param maxProgress 
	 * @author zhuanggy
	 * @date 2014-3-28
	 */
	public void setMaxProgress(int maxProgress) {
		mHasLeftItems = false;
		mScrollBar.setMaxProgress(maxProgress);
	}

	/**
	 * 设置滚动Bar的高度至少是其宽度的几倍，默认为1.0f倍，（因为若数据量非常大的话，滚动bar高度会非常细）
	 * @param @param multiple 
	 * @author zhuanggy
	 * @date 2014-3-28
	 */
	public void setMinBarHeightMultipleWidth(float multiple) {
		mScrollBar.setMinBarHeightMultipleWidth(multiple);
	}
	
	/**
	 * 获得ListView
	 * 
	 * (注意：不能通过getListView().OnScrollListener()，只能通过 {@link setOnListViewScrollListener(AbsListView.OnScrollListener)方法设置监听} )
	 * 
	 * @param @return 
	 * @author zhuanggy
	 * @date 2014-3-28
	 */
	public ListView getListView() {
		return mListView;
	}

	/**
	 * 你没必要获得滚动条，不需要管它
	 * @param @return 
	 * @author zhuanggy
	 * @date 2014-3-28
	 */
	@Deprecated
	public ScrollBar getScrollBar() {
		return null;
	}
	/**
	 * 设置滚动条监听器
	 * 
	 * @param @param listen 
	 * @author zhuanggy
	 * @date 2014-3-28
	 */
	public void setOnBarScrollListener(ScrollBar.OnBarListener listen) {
		this.mOnBarScrollListener = listen;
	}

	/**
	 * 设置ListView的滚动监听器
	 * 
	 * @param @param listen 
	 * @author zhuanggy
	 * @date 2014-3-28
	 */
	public void setOnListViewScrollListener(AbsListView.OnScrollListener listen) {
		this.mOnListViewScrollListener = listen;
	}
}
