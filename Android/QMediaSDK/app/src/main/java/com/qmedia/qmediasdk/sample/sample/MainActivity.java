package com.qmedia.qmediasdk.sample.sample;

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
import com.qmedia.qmediasdk.sample.R;

public class MainActivity extends AppCompatActivity {

	private static final String TAG = "MAINACTIVITY";

	//private final int CONTEXT_CLIENT_VERSION = 3;
	//private GLSurfaceView mGLSurfaceView;

//	OutputMuxer mOutputMuxer = new OutputMuxer();

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		Button button_audio = (Button)findViewById(R.id.button_audio);
		button_audio.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(MainActivity.this, PreviewMuxerActivity.class);
//				intent.putExtra("LAUNCH_TYPE", Constant.LAUNCH_TYPE_MERGE);
				Log.d(TAG, "LAUNCH_TYPE_MERGE");
				startActivity(intent);

			}
		});

		String path = Environment.getExternalStorageDirectory().getPath();
		if (!(new File(path + "/test.mp4").exists())) {
			Log.d(TAG, "MainActivity copyfile" + path + "/test2.m4v");
			copyDBToSD(this, path, "test.mp4");
		}
		if (!(new File(path + "/test2.m4v").exists())) {
			Log.d(TAG, "MainActivity copyfile" + path + "/test2.m4v");
			copyDBToSD(this, path, "test2.m4v");
		}

		if (!(new File(path + "/ID_24.bmp").exists())) {
			Log.d(TAG, "MainActivity copyfile" + path + "/ID_24.bmp");
			copyDBToSD(this, path, "ID_24.bmp");
		}

		if (!(new File(path + "/audio.mp3").exists())) {
			Log.d(TAG, "MainActivity copyfile" + path + "/audio.mp3");
			copyDBToSD(this, path, "audio.mp3");
		}
		Log.d(TAG, "oncreate getStoragePath=" + path);

		//MediaMuxerTest.MediaMuxer("/sdcard/test.mp4", "/sdcard/test_out.mp4");
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
