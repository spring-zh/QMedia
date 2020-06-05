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
import com.qmedia.qmediasdk.sample.R;

public class MainActivity extends AppCompatActivity {

	private static final String TAG = "MainActivity";

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		Button button_audio = (Button)findViewById(R.id.button_preview);
		button_audio.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(MainActivity.this, PreviewMuxerActivity.class);
//				intent.putExtra("LAUNCH_TYPE", Constant.LAUNCH_TYPE_MERGE);
				Log.d(TAG, "LAUNCH_TYPE_MERGE");
				startActivity(intent);

			}
		});
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
