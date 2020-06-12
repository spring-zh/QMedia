package com.qmedia.qmediasdk.sample;

import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;

import com.lcw.library.imagepicker.ImagePicker;
import com.qmedia.qmediasdk.sample.R;

public class MainActivity extends AppCompatActivity {

	private static final String TAG = "MainActivity";
	private final int REQUEST_SELECT_VIDEO_CODE = 111;
	private final int REQUEST_SELECT_IMAGESCODE = 222;

	public static Context context;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		context = this.getApplicationContext();

		Button button_preview = (Button)findViewById(R.id.button_preview);
		button_preview.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(MainActivity.this, PreviewMuxerActivity.class);
//				intent.putExtra("LAUNCH_TYPE", Constant.LAUNCH_TYPE_MERGE);
				Log.d(TAG, "LAUNCH_TYPE_MERGE");
				startActivity(intent);

			}
		});


		Button button_select_video = (Button)findViewById(R.id.button_select_video);
		button_select_video.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				ImagePicker.getInstance()
						.setTitle("标题")//设置标题
						.showCamera(true)//设置是否显示拍照按钮
						.showImage(false)//设置是否展示图片
						.showVideo(true)//设置是否展示视频
						.setSingleType(true)//设置图片视频不能同时选择
						.setMaxCount(1)//设置最大选择图片数目(默认为1，单选)
						.setImageLoader(new GlideLoader())//设置自定义图片加载器
						.start(MainActivity.this, REQUEST_SELECT_VIDEO_CODE);//REQEST_SELECT_IMAGES_CODE为Intent调用的requestCode

			}
		});

		Button button_select_images = (Button)findViewById(R.id.button_select_images);
		button_select_images.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				ImagePicker.getInstance()
						.setTitle("选择媒体")//设置标题
						.showCamera(true)//设置是否显示拍照按钮
						.showImage(true)//设置是否展示图片
						.showVideo(false)//设置是否展示视频
						.setSingleType(true)//设置图片视频不能同时选择
						.setMaxCount(4)//设置最大选择图片数目(默认为1，单选)
						.setImageLoader(new GlideLoader())//设置自定义图片加载器
						.start(MainActivity.this, REQUEST_SELECT_IMAGESCODE);//REQEST_SELECT_IMAGES_CODE为Intent调用的requestCode

			}
		});
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if ( resultCode == RESULT_OK) {
			List<String> mediaPaths = data.getStringArrayListExtra(ImagePicker.EXTRA_SELECT_IMAGES);
			String[] paths = new String[mediaPaths.size()];
			if (mediaPaths.size() > 0) {
				for (int i=0;i<mediaPaths.size();i++) {
					paths[i] = mediaPaths.get(i);
				}
			}
			if (requestCode == REQUEST_SELECT_VIDEO_CODE) {

				Intent intent = new Intent(MainActivity.this, PreviewMuxerActivity.class);
				intent.putExtra("MEDIA_PATHS", paths);
				intent.putExtra("LAUNCH_MODE", "SELECT_VIDEO");
				startActivity(intent);
			}else if (requestCode == REQUEST_SELECT_IMAGESCODE) {
				Intent intent = new Intent(MainActivity.this, PreviewMuxerActivity.class);
				intent.putExtra("MEDIA_PATHS", paths);
				intent.putExtra("LAUNCH_MODE", "SELECT_IMAGES");
				startActivity(intent);
			}
		}
	}

	public static void copyDBToSD(Context context, String path, String filename) {
		AssetManager am = context.getAssets();
		try {
			InputStream inputStream = am.open(filename);
			File file = new File(path, filename);
			FileOutputStream fos = new FileOutputStream(file);
			int len = 0;
			byte[] buffer = new byte[1024];
			while ((len=inputStream.read(buffer))!=-1) {
				fos.write(buffer,0,len);
			}

			fos.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}


	@Override
	protected void onDestroy() {
		super.onDestroy();

		//XMNative.module_destroy();
	}

	private int cnt = 0;
}
