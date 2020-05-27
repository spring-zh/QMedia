package com.qmedia.qmediasdk.sample.sample.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.qmedia.qmediasdk.sample.R;

import java.util.List;
import java.util.Map;

public class HorizontalScrollViewAdapter
{

	private Context mContext;
	private LayoutInflater mInflater;
	private List<Map<String, Object>> mDatas;

	public HorizontalScrollViewAdapter(Context context, List<Map<String, Object>> mDatas)
	{
		this.mContext = context;
		mInflater = LayoutInflater.from(context);
		this.mDatas = mDatas;
	}

	public int getCount()
	{
		return mDatas.size();
	}

	public Object getItem(int position)
	{
		return mDatas.get(position);
	}

	public long getItemId(int position)
	{
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent)
	{
		ViewHolder viewHolder = null;
		if (convertView == null)
		{
			viewHolder = new ViewHolder();
			convertView = mInflater.inflate(
					R.layout.hororizontal_index_item, parent, false);
			viewHolder.mImg = (ImageView) convertView
					.findViewById(R.id.id_index_gallery_item_image);
			viewHolder.mText = (TextView) convertView
					.findViewById(R.id.id_index_gallery_item_text);

			convertView.setTag(viewHolder);
		} else
		{
			viewHolder = (ViewHolder) convertView.getTag();
		}
		viewHolder.mImg.setImageResource(R.drawable.filter_img);
		viewHolder.mText.setText((String)mDatas.get(position).get(FilterListViewAdapter.FILTER_NAME_TOKEN));
		return convertView;
	}

	private class ViewHolder
	{
		ImageView mImg;
		TextView mText;
	}

}
