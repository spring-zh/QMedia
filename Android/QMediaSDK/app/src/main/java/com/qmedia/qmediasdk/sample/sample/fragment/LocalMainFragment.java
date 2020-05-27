package com.qmedia.qmediasdk.sample.sample.fragment;

import android.os.Build;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.view.ViewPager;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.qmedia.qmediasdk.sample.R;
import com.qmedia.qmediasdk.sample.sample.base.BaseFragment;
import com.qmedia.qmediasdk.sample.sample.bean.EventCenter;
import com.qmedia.qmediasdk.sample.sample.bean.FileInfo;
import com.qmedia.qmediasdk.sample.sample.utils.SystemUtil;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;

import butterknife.Bind;
import butterknife.OnClick;

/**
 * Created by CWJ on 2017/3/28.
 */

public class LocalMainFragment extends BaseFragment {
    @Bind(R.id.tl_myfile)
    TabLayout mTabLayout;
    @Bind(R.id.vp_myfile)
    ViewPager mViewPager;
    private List<String> mTitleList = new ArrayList<>();
    private List<Fragment> fragments = new ArrayList<>();
    @Bind(R.id.tv_all_size)
    TextView tv_all_size;
    @Bind(R.id.tv_send)
    TextView tv_send;
    @Bind(R.id.tv_preview)
    TextView tv_preview;
    List<FileInfo> mListphoto = new ArrayList<>();

    @OnClick(R.id.tv_preview)
    void tv_preview() {
//        if (mListphoto.size() != 0) {
//            Intent intent = new Intent(getActivity(), ImagePreviewActivity.class);
//            intent.putExtra("FileInfo", (ArrayList) mListphoto);
//            startActivity(intent);
//        }
    }

    @Override
    public void onEventComming(EventCenter var1) {
        switch (var1.getEventCode()) {
            case 1://点击 选中和不选中都通知更新一下
                updateSizAndCount();
                break;
            case 2://fragment切换更新底部视图，是否有预览项
                if ((int) var1.getData() == 1) {//当前为相册fragment显示预览
                    tv_preview.setVisibility(View.VISIBLE);
                } else {
                    tv_preview.setVisibility(View.GONE);
                }
                break;
        }


    }

    @Override
    public boolean isBindEventBusHere() {
        return true;
    }

    @Override
    public int getLayoutResource() {
        return R.layout.a_fragment_main_local;
    }

    @Override
    public void initView() {

        /*
        tv_all_size.setText(getString(R.string.size, "0B"));
        tv_send.setText(getString(R.string.send, "0"));
        updateSizAndCount();
        mTitleList.add("影音");
        mTitleList.add("图片");
        mTitleList.add("文档");
        mTitleList.add("其他");

        fragments.add(new AVFragment());
        fragments.add(new PhotoFragment());
        fragments.add(new DocFragment());
        fragments.add(new OtherFragment());
        TabPagerAdapter mAdapter = new TabPagerAdapter(getChildFragmentManager(), mTitleList, fragments);
        mViewPager.setAdapter(mAdapter);
        mViewPager.setCurrentItem(0);
        mViewPager.setOffscreenPageLimit(4);
        mTabLayout.setupWithViewPager(mViewPager);
        mTabLayout.setTabsFromPagerAdapter(mAdapter);
        mViewPager.addOnPageChangeListener(new TabLayout.TabLayoutOnPageChangeListener(mTabLayout));
        mTabLayout.addOnTabSelectedListener(new TabLayout.OnTabSelectedListener() {
            @Override
            public void onTabSelected(TabLayout.Tab tab) {
                mViewPager.setCurrentItem(tab.getPosition(), false);
                EventBus.getDefault().post(new EventCenter<>(2, tab.getPosition()));
            }

            @Override
            public void onTabUnselected(TabLayout.Tab tab) {

            }

            @Override
            public void onTabReselected(TabLayout.Tab tab) {

            }
        });
        //反射修改宽度
        setUpIndicatorWidth(mTabLayout);

        */
    }

    public void updateSizAndCount() {}

    private void setUpIndicatorWidth(TabLayout mTabLayout) {
        Class<?> tabLayoutClass = mTabLayout.getClass();
        Field tabStrip = null;
        try {
            tabStrip = tabLayoutClass.getDeclaredField("mTabStrip");
            tabStrip.setAccessible(true);
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }

        LinearLayout layout = null;
        try {
            if (tabStrip != null) {
                layout = (LinearLayout) tabStrip.get(mTabLayout);
            }
            for (int i = 0; i < layout.getChildCount(); i++) {
                View child = layout.getChildAt(i);
                child.setPadding(0, 0, 0, 0);
                LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(0, LinearLayout.LayoutParams.MATCH_PARENT, 1);
                if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
                    params.setMarginStart(SystemUtil.dp(30f));
                    params.setMarginEnd(SystemUtil.dp(30f));
                }
                child.setLayoutParams(params);
                child.invalidate();
            }
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        }
    }
}
