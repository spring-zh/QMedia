package com.qmedia.qmediasdk.sample.sample.activity;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentTransaction;
//import android.app.FragmentManager;
//import android.app.FragmentTransaction;

import android.support.v7.app.AppCompatActivity;

import com.ashokvarma.bottomnavigation.BadgeItem;
import com.ashokvarma.bottomnavigation.BottomNavigationBar;
import com.ashokvarma.bottomnavigation.BottomNavigationItem;
import com.qmedia.qmediasdk.sample.sample.fragment.LocationFragment;
import com.qmedia.qmediasdk.sample.R;

/**
 * Created by ts on 2017/5/22.
 */

public class Navigation extends AppCompatActivity implements BottomNavigationBar.OnTabSelectedListener{

	private int lastSelectedPosition;
	private int mHomeMessage;
	private BottomNavigationBar bottomNavigationBar;
	private BadgeItem mHomeNumberBadgeItem,mMusicNumberBadgeItem;

//	private HomeFragment mHomeFragment;
//	private BookFragment mBookFragment;
//	private MusicFragment mMusicFragment;
//	private FavoriteFragment mFavoriteFragment;

	private LocationFragment mHomeFragment;
	private LocationFragment mBookFragment;
	private LocationFragment mMusicFragment;
	private LocationFragment mFavoriteFragment;

	private FragmentManager mFragmentManager;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.navigation);
		initView();
		setDefaultFragment();
	}

	private void initView() {

		/**
		 *添加标签的消息数量
		 */
		mHomeNumberBadgeItem = new BadgeItem()
				.setBorderWidth(2)
				.setBackgroundColor(Color.RED)
				.setText("")
				.setHideOnSelect(false); // TODO 控制便签被点击时 消失 | 不消失


		/**
		 *添加标签的消息数量
		 */
		mMusicNumberBadgeItem = new BadgeItem()
				.setBorderWidth(2)
				.setBackgroundColor(Color.RED)
				.setText("99+")
				.setHideOnSelect(true);


		bottomNavigationBar = (BottomNavigationBar) findViewById(R.id.bottom_navigation_bar);

		// TODO 设置模式
		bottomNavigationBar
				.setMode(BottomNavigationBar.MODE_FIXED);
		// TODO 设置背景色样式
		bottomNavigationBar
				.setBackgroundStyle(BottomNavigationBar.BACKGROUND_STYLE_RIPPLE);

		bottomNavigationBar.setTabSelectedListener(this);

		bottomNavigationBar
				.addItem(new BottomNavigationItem(R.mipmap.ic_launcher
						/*ic_home_white_24dp*/, "编辑").setActiveColorResource(R.color.orange)
						.setBadgeItem(mHomeNumberBadgeItem))
				.addItem(new BottomNavigationItem(R.mipmap.ic_launcher /*ic_book_white_24dp*/, "滤镜")
						.setActiveColorResource(R.color.teal))
				.addItem(new BottomNavigationItem(R.mipmap.ic_launcher /*ic_music_note_white_24dp*/, "变换")
						.setActiveColorResource(R.color.blue)/*.setBadgeItem(mMusicNumberBadgeItem)*/)
				.addItem(new BottomNavigationItem(R.mipmap.ic_launcher /*ic_favorite_white_24dp*/, "区域动画")
						.setActiveColorResource(R.color.brown))
				.setFirstSelectedPosition(lastSelectedPosition)
				.initialise();

		// TODO 设置 BadgeItem 默认隐藏 注意 这句代码在添加 BottomNavigationItem 之后
		mHomeNumberBadgeItem.hide();
	}

	private void setDefaultFragment() {

		mFragmentManager = getSupportFragmentManager();
		FragmentTransaction transaction = mFragmentManager.beginTransaction();
		mHomeFragment = mHomeFragment.newInstance("HomeFragment");
		transaction.add(R.id.tb, mHomeFragment);
		transaction.commit();

	}

	@Override
	public void onTabSelected(int position) {
		lastSelectedPosition = position;

		//开启事务
		FragmentTransaction transaction = mFragmentManager.beginTransaction();
		hideFragment(transaction);

		/**
		 * fragment 用 add + show + hide 方式
		 * 只有第一次切换会创建fragment，再次切换不创建
		 *
		 * fragment 用 replace 方式
		 * 每次切换都会重新创建
		 *
		 */
		switch (position) {
			case 0:
				if (mHomeFragment == null) {
					mHomeFragment = mHomeFragment.newInstance("HomeFragment");
					transaction.add(R.id.tb, mHomeFragment);
				}else{
					transaction.show(mHomeFragment);
				}
//                transaction.replace(R.id.tb, mHomeFragment);
				break;
			case 1:
				if (mBookFragment == null) {
					mBookFragment = /*BookFragment*/LocationFragment.newInstance("mBookFragment");
					transaction.add(R.id.tb, mBookFragment);
				}else{
					transaction.show(mBookFragment);
				}
//                transaction.replace(R.id.tb, mBookFragment);
				break;
			case 2:
				if (mMusicFragment == null) {
					mMusicFragment = /*MusicFragment*/LocationFragment.newInstance("mMusicFragment");
					transaction.add(R.id.tb, mMusicFragment);
				}else{
					transaction.show(mMusicFragment);
				}
//                transaction.replace(R.id.tb, mMusicFragment);
				break;
			case 3:
				if (mFavoriteFragment == null) {
					mFavoriteFragment = /*FavoriteFragment*/LocationFragment.newInstance("mFavoriteFragment");
					transaction.add(R.id.tb, mFavoriteFragment);
				}else{
					transaction.show(mFavoriteFragment);
				}
//                transaction.replace(R.id.tb, mFavoriteFragment);
				break;
			default:
				break;
		}
		// 事务提交
		transaction.commit();
	}

	/**
	 * 隐藏当前fragment
	 * @param transaction
	 */
	private void hideFragment(FragmentTransaction transaction){
		if (mHomeFragment != null){
			transaction.hide(mHomeFragment);
		}
		if (mBookFragment != null){
			transaction.hide(mBookFragment);
		}
		if (mMusicFragment != null){
			transaction.hide(mMusicFragment);
		}
		if (mFavoriteFragment != null){
			transaction.hide(mFavoriteFragment);
		}
	}

	@Override
	public void onTabUnselected(int position) {

	}

	@Override
	public void onTabReselected(int position) {

	}

	public void addMessage(){
		mHomeMessage ++ ;
		mHomeNumberBadgeItem.setText(mHomeMessage + "");
		mHomeNumberBadgeItem.show();
	}
}
