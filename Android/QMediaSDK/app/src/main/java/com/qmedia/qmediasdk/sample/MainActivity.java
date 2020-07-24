package com.qmedia.qmediasdk.sample;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Build;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.annotation.RequiresApi;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.lcw.library.imagepicker.ImagePicker;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

	private static final String TAG = "MainActivity";
	private final int REQUEST_SELECT_VIDEO_CODE = 111;
	private final int REQUEST_SELECT_IMAGESCODE = 222;

	public static Context context;

	String[] permissions = new String[]{
			Manifest.permission.WRITE_EXTERNAL_STORAGE,
			Manifest.permission.READ_EXTERNAL_STORAGE,
	};
	List<String> mPermissionList = new ArrayList<>();

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

		if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
			askpermisson();

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

	//请求图片存储本地的权限
	@RequiresApi(api = Build.VERSION_CODES.M)
	public void askpermisson(){
		/**
		 * 获取零时权限
		 */
		mPermissionList.clear();
		for (int i = 0; i < permissions.length; i++) {
			if (ContextCompat.checkSelfPermission(this, permissions[i]) != PackageManager.PERMISSION_GRANTED) {
				mPermissionList.add(permissions[i]);
			}
		}
		if (!mPermissionList.isEmpty()) {

			String[] permissions = mPermissionList.toArray(new String[mPermissionList.size()]);//将List转为数组
			requestPermissions( permissions, 9);
		}
	}

	@Override
	public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
		if (requestCode == 9) {
			if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
			} else {
				Log.e(TAG, "onRequestPermissionsResult: 申请权限已拒绝");
			}
		}
		super.onRequestPermissionsResult(requestCode, permissions, grantResults);
	}
}
