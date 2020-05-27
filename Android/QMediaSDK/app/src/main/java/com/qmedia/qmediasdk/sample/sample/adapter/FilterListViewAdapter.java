package com.qmedia.qmediasdk.sample.sample.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.qmedia.qmediasdk.sample.R;
import com.qmedia.qmediasdk.sample.sample.utils.SystemUtil;
import com.qmedia.qmediasdk.sample.sample.view.VideoSeekBar;

import java.util.List;
import java.util.Map;

/**
 * Created by spring on 2017/6/30.
 */

public class FilterListViewAdapter extends BaseAdapter{
    public static final String FILTER_NAME_TOKEN = "filterName";
    public static final String FILTER_BEGIN_TIME_MS = "beginTime";
    public static final String FILTER_END_TIME_MS = "endTime";
    private Context mContext;
    private List<Map<String, Object>> mData;

    public FilterListViewAdapter(Context context, List<Map<String, Object>> data) {
        this.mContext = context;
        mData = data;
    }

    public void notifyDataSetChanged(List<Map<String, Object>> data) {
        mData = data;
        super.notifyDataSetChanged();
    }

    public int getCount() {
        return mData.size();
    }

    public Object getItem(int position) {
        return null;
    }

    public long getItemId(int position) {
        return 0;
    }

    public View getView(int position, View convertView, ViewGroup parent) {

        ViewHolder holder = null;
        if (convertView == null) {
            convertView = LayoutInflater.from(mContext).inflate(R.layout.filter_list_items_layout, null);
            holder = new ViewHolder();
            holder.FilterName = (TextView) convertView.findViewById(R.id.filter_name);
            holder.FilterBeginTime = (TextView) convertView.findViewById(R.id.filter_begin_time);
            holder.FilterEndTime = (TextView) convertView.findViewById(R.id.filter_end_time);
            holder.FilterBar = (VideoSeekBar) convertView.findViewById(R.id.filter_seekbar);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }
        holder.FilterName.setText((String) mData.get(position).get(FILTER_NAME_TOKEN));
        holder.FilterBeginTime.setText(SystemUtil.getForamtedTime((Integer) mData.get(position).get(FILTER_BEGIN_TIME_MS)));
        holder.FilterEndTime.setText(SystemUtil.getForamtedTime((Integer)mData.get(position).get(FILTER_END_TIME_MS)));
        holder.FilterBar.showProgressLine(false);
        holder.FilterBar.showProgressBG(false);
        holder.FilterBar.showSlideBar(true);
        holder.FilterBar.setClickable(true);

        return convertView;
    }


    private static class ViewHolder {
        public TextView FilterName;
        public TextView FilterBeginTime;
        public TextView FilterEndTime;
        public VideoSeekBar FilterBar;
    }
}
