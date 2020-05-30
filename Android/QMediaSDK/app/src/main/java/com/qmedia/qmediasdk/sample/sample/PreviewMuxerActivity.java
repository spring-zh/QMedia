package com.qmedia.qmediasdk.sample.sample;

import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.Toast;

import java.io.File;

import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QEditor.QCombiner;
import com.qmedia.qmediasdk.QEditor.QEditorPlayer;
import com.qmedia.qmediasdk.QEditor.QExporter;
import com.qmedia.qmediasdk.QGraphic.QDuplicateNode;
import com.qmedia.qmediasdk.QGraphic.QNodeAnimator;
import com.qmedia.qmediasdk.QMediaSDK;
import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;
import com.qmedia.qmediasdk.QTarget.Implements.QFileExporterTarget;
import com.qmedia.qmediasdk.QTarget.Implements.QPlayerView;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;
import com.qmedia.qmediasdk.sample.R;

public class PreviewMuxerActivity extends AppCompatActivity implements View.OnClickListener{

	private final String TAG = "PreviewView";
	ProgressBar mProgressBar;
	SeekBar mSeekBar;
	boolean mIsTrack = false;

	QPlayerView mPreviewView;
	FrameLayout mframeLayout;

	final int targetW = 640;
    final int targetH = 480;
	boolean bfileEnd = false;

	//    Timer mTimer;//process timer
	Handler mUIHandler;

	QEditorPlayer editorPlayer = new QEditorPlayer();

	QExporter exporter;

	void setEffects(QCombiner combiner) {
		combiner.getRootNode().setBKColor(new QVector(0,0,1,1));

//		String path = Environment.getExternalStorageDirectory().getPath() + "/test.mp4";
		String path = "test.mp4";
		QMediaTrack videoTrack = combiner.createVideoTrack(path, true);
		videoTrack.getGraphic().setPosition(new QVector(targetW/4, targetH/4));
		videoTrack.getGraphic().setContentSize(new QVector(targetW/2, targetH/2));
		videoTrack.getGraphic().setAnchorPoint(new QVector(0.5f,0.5f));

		QDuplicateNode duplicatenodeL = new QDuplicateNode(videoTrack.getGraphic(), combiner);
		duplicatenodeL.setContentSize(new QVector(targetW/2, targetH/2));
		duplicatenodeL.setPosition(new QVector(0, targetH/4));
		duplicatenodeL.setAnchorPoint(new QVector(0.5f,0.5f));
		duplicatenodeL.setRotation3d(new QVector(0, 90, 0));

		QDuplicateNode duplicatenodeR = new QDuplicateNode(videoTrack.getGraphic(), combiner);
		duplicatenodeR.setContentSize(new QVector(targetW/2, targetH/2));
		duplicatenodeR.setPosition(new QVector(targetW/2, targetH/4));
		duplicatenodeR.setAnchorPoint(new QVector(0.5f,0.5f));
		duplicatenodeR.setRotation3d(new QVector(0, -90, 0));

		combiner.getRootNode().setAnchorPoint(new QVector(0.5f,0.5f));
		combiner.getRootNode().setContentSize(new QVector(targetW, targetH));
		combiner.getRootNode().addChildNode(videoTrack.getGraphic());
		combiner.getRootNode().addChildNode(duplicatenodeL);
		combiner.getRootNode().addChildNode(duplicatenodeR);


		QNodeAnimator an1 = new QNodeAnimator(QNodeAnimator.rotationxyz, new QRange(0, 5000) ,
				new QVector(0, 0, 0) , new QVector(-180, 180, 180), QNodeAnimator.Linear ,false, "");
		QNodeAnimator an2 = new QNodeAnimator(QNodeAnimator.rotationxyz, new QRange(5000, 10000) ,
				new QVector(-180, 180, 180) , new QVector(-360, 360, 360), QNodeAnimator.Linear ,false, "");
		combiner.getRootNode().addAnimator(an1);
		combiner.getRootNode().addAnimator(an2);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_reader_muxer);
//		Log.i(TAG, "qmediasdk version: " + QMediaSDK.SDK_VERSION);
		QMediaSDK.init(this);
		mframeLayout = (FrameLayout) findViewById(R.id.frameLayout);
		mPreviewView = (QPlayerView) findViewById(R.id.render_view);
		mPreviewView.setDisplayMode(QPlayerView.DisplayMode.Adaptive);

		Log.e(TAG, "activity_create width=" + mPreviewView.getWidth() + " height=" + mPreviewView.getHeight() + " " + QMediaSDK.SDK_VERSION);

		Button btn_start = (Button) findViewById(R.id.btn_start);
		Button btn_stop = (Button) findViewById(R.id.btn_stop);
		Button btn_save = (Button) findViewById(R.id.btn_save);
		Button btn_capture = (Button) findViewById(R.id.btn_capture);

		btn_start.setOnClickListener(this);
		btn_stop.setOnClickListener(this);
		btn_save.setOnClickListener(this);
		btn_capture.setOnClickListener(this);

		mProgressBar = (ProgressBar) findViewById(R.id.bar_progress);
		mProgressBar.setMax(100);
		mSeekBar = (SeekBar) findViewById(R.id.seekBar_process);
		mSeekBar.setMax(100);
		mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				if (fromUser) {
					long position = progress * editorPlayer.getMediaTimeRange().getLength() / 100;
					editorPlayer.seekTo(position, 0);
				}
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				mIsTrack = true;
			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				long position = mSeekBar.getProgress() * editorPlayer.getMediaTimeRange().getLength() / 100;
				editorPlayer.seekTo(position, 1);
				mIsTrack = false;

			}
		});

		//set preview and resource
		editorPlayer.setPlayerView(mPreviewView);
		editorPlayer.setObserver(new QEditorPlayer.Observer() {
			@Override
			public void onPrepare() {

			}

			@Override
			public void onPlayerStateChanged(int newState, int oldState) {

			}

			@Override
			public void onPlayerTimeProgressUpdated(long progress) {
				QRange range = editorPlayer.getMediaTimeRange();
				mSeekBar.setProgress((int) (editorPlayer.getPosition() * 100 / range.getLength()));
			}

			@Override
			public void onSeekTo(long mSec) {

			}

			@Override
			public void onCompleted(int errcode) {
				Log.i(TAG, "onCompleted");
			}
		});

		//TODO: set resources
		setEffects(editorPlayer);

		editorPlayer.start();
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();

		mPreviewView.onDestroy();
		editorPlayer.stop();
		editorPlayer.release();

	}

	static int runtime = 0;

	@Override
	protected void onResume() {
		super.onResume();

		mPreviewView.onResume();
	}

	@Override
	protected void onPause() {
		super.onPause();
		mPreviewView.onPause();
	}


	@Override
	public void onClick(View v) {
		switch (v.getId()) {
			case R.id.btn_start: {

				editorPlayer.play();
			}
			break;
			case R.id.btn_stop: {

				editorPlayer.pause();

			}
			break;
			case R.id.btn_save: {

				if (exporter != null)
					return;

				String savePath = "/sdcard/output.mp4";
				File f = new File(savePath);
				if(f.exists()){
					f.delete();
				}

				QFileExporterTarget fileExporterTarget = new QFileExporterTarget(savePath);
				fileExporterTarget.setEnableAudio(true);
				exporter = new QExporter(fileExporterTarget,fileExporterTarget);
				exporter.setAudioConfig(new QAudioDescribe(QAudioDescribe.QAudioCodecAAC, QAudioDescribe.QRawAudioFormatS16,
						44100, 2, 64000));
				exporter.setVideoConfig(new QVideoDescribe(QVideoDescribe.QVideoCodecH264, QVideoDescribe.QRawVideoFormatHardware,
						QVideoDescribe.QVideoRotation_0, 25, 640, 480, 1*1024*1024, 1000));
				exporter.setObserver(new QExporter.Observer() {
					@Override
					public void onExporterStarted(int code) {
						
					}

					@Override
					public void onExporterStoped(int code) {
						mProgressBar.setVisibility(View.INVISIBLE);
						exporter.release();
						exporter = null;
						Toast.makeText(PreviewMuxerActivity.this, "onExporterStoped", Toast.LENGTH_SHORT).show();
					}

					@Override
					public void onExporterProgressUpdated(long progress) {
						QRange range = editorPlayer.getMediaTimeRange();
						mProgressBar.setProgress((int) (exporter.getPosition() * 100 / range.getLength()));
					}

					@Override
					public void onExporterCanceled(int code) {
						mProgressBar.setVisibility(View.INVISIBLE);
						exporter.release();
						exporter = null;
						Toast.makeText(PreviewMuxerActivity.this, "onExporterCanceled", Toast.LENGTH_SHORT).show();
					}

					@Override
					public void onExporterCompleted() {
						mProgressBar.setVisibility(View.INVISIBLE);
						exporter.release();
						exporter = null;
						Toast.makeText(PreviewMuxerActivity.this, "onExporterCompleted", Toast.LENGTH_SHORT).show();
					}
				});
				setEffects(exporter);
				exporter.start();


				mProgressBar.setVisibility(View.VISIBLE);
				mProgressBar.setMax(100);
				mProgressBar.setIndeterminate(false);
			}
			break;
			case R.id.btn_capture:
				break;
		}
	}
}
