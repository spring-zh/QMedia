package com.qmedia.qmediasdk.sample.sample.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.qmedia.qmediasdk.sample.R;
import com.qmedia.qmediasdk.sample.sample.view.ScrollBar;

import java.util.List;
import java.util.Map;

/**
 * Created by ts on 2017/5/23.
 */

public class ListViewAdapter  extends BaseAdapter {
	public static final String CLIP_NAME_TOKEN = "fileName";
	public static final String THUMB_WIDTH_TOKEN = "scrollBarWidth";
	private Context mContext;
	//private String[] texts;
	private List<Map<String, Object>> mData;

	public ListViewAdapter(Context context, List<Map<String, Object>> data) {
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
			convertView = LayoutInflater.from(mContext).inflate(R.layout.list_item, null);
			holder = new ViewHolder();
			holder.mTextView = (TextView) convertView.findViewById(R.id.text);
			holder.mScrollBar = (ScrollBar) convertView.findViewById(R.id.scrollbar);
			convertView.setTag(holder);
		} else {
			holder = (ViewHolder) convertView.getTag();
		}
		holder.mScrollBar.setBarRelativeWH((float) mData.get(position).get(THUMB_WIDTH_TOKEN), 1.0f);
		holder.mTextView.setText((String) mData.get(position).get(CLIP_NAME_TOKEN));
		return convertView;
	}


	private static class ViewHolder {
		public TextView mTextView;
		public ScrollBar mScrollBar;
	}
}
