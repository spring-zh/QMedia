package com.qmedia.qmediasdk.sample;

import android.media.effect.Effect;
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
import android.widget.TextView;
import android.widget.Toast;

import java.io.File;
import java.util.UUID;
import java.util.Vector;

import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QCommon.QSize;
import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QEditor.QCombiner;
import com.qmedia.qmediasdk.QEditor.QEditorPlayer;
import com.qmedia.qmediasdk.QEditor.QExporter;
import com.qmedia.qmediasdk.QEditor.QFileExporter;
import com.qmedia.qmediasdk.QEffect.QEffect;
import com.qmedia.qmediasdk.QEffect.QEffectConfig;
import com.qmedia.qmediasdk.QEffect.QEffectManage;
import com.qmedia.qmediasdk.QGraphic.QDuplicateNode;
import com.qmedia.qmediasdk.QGraphic.QGraphicNode;
import com.qmedia.qmediasdk.QGraphic.QImageNode;
import com.qmedia.qmediasdk.QGraphic.QLayer;
import com.qmedia.qmediasdk.QGraphic.QNodeAnimator;
import com.qmedia.qmediasdk.QMediaSDK;
import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;
import com.qmedia.qmediasdk.QTarget.Implements.QPlayerView;
import com.qmedia.qmediasdk.QTrack.QMediaTrack;
import com.qmedia.qmediasdk.sample.R;

public class PreviewMuxerActivity extends AppCompatActivity implements View.OnClickListener{

	private final String TAG = "PreviewView";
	TextView mProgressBar;
	SeekBar mSeekBar;
	boolean mIsTrack = false;

	QPlayerView mPreviewView;
	FrameLayout mframeLayout;

	final int targetW = 640;
    final int targetH = 480;
	boolean bfileEnd = false;

	QEditorPlayer editorPlayer = new QEditorPlayer();

	QFileExporter exporter;

	String[] media_paths;

	void setVideoPreview(QCombiner combiner , String video_path) {
		combiner.getRootNode().setBkColor(new QVector(0,0,1,1));

//		String path = Environment.getExternalStorageDirectory().getPath() + "/test.mp4";
		QMediaTrack videoTrack ;
		if (video_path != null) {
			videoTrack = combiner.createVideoTrack(video_path, false);
		}else
			videoTrack = combiner.createVideoTrack("test.mp4", true);
		QMediaTrack audioTrack = combiner.createAudioTrack("LR.mp3", true);

		videoTrack.getGraphic().setPosition(new QVector(targetW/4, targetH/4));
		videoTrack.getGraphic().setContentSize(new QSize(targetW/2, targetH/2));
		videoTrack.getGraphic().setAnchorPoint(new QVector(0.5f,0.5f));

		QDuplicateNode duplicatenodeL = new QDuplicateNode(videoTrack.getGraphic(), combiner);
		duplicatenodeL.setContentSize(new QSize(targetW/2, targetH/2));
		duplicatenodeL.setPosition(new QVector(0, targetH/4));
		duplicatenodeL.setAnchorPoint(new QVector(0.5f,0.5f));
		duplicatenodeL.setRotation3d(new QVector(0, 90, 0));

		QDuplicateNode duplicatenodeR = new QDuplicateNode(videoTrack.getGraphic(), combiner);
		duplicatenodeR.setContentSize(new QSize(targetW/2, targetH/2));
		duplicatenodeR.setPosition(new QVector(targetW/2, targetH/4));
		duplicatenodeR.setAnchorPoint(new QVector(0.5f,0.5f));
		duplicatenodeR.setRotation3d(new QVector(0, -90, 0));

		QGraphicNode composeNode = new QGraphicNode("composeNode", combiner);
		composeNode.setAnchorPoint(new QVector(0.5f,0.5f));
		composeNode.setContentSize(new QSize(targetW, targetH));
		composeNode.addChildNode(videoTrack.getGraphic());
		composeNode.addChildNode(duplicatenodeL);
		composeNode.addChildNode(duplicatenodeR);

//		String imagePath = Environment.getExternalStorageDirectory().getPath() + "/li.jpg";
//		QImageNode imageNode = new QImageNode(imagePath, false, combiner);
//		imageNode.setRenderRange(new QRange(5000, 15000));
//		imageNode.setContentSize(new QVector(320, 240));
//		combiner.getRootNode().addChildNode(imageNode);

		QEffectConfig[] allconfig = QEffectManage.getAllEffectConfig();
		QEffect effect = QEffectManage.createEffect("SobelEdgeDetection");
		effect.setRenderRange(new QRange(0,10000));

//		QLayer layer = new QLayer(new QSize(targetW, targetH), "", combiner);
//		layer.setEnable3d(true);
//		layer.setContentSize(new QSize(targetW, targetH));
//		layer.setAnchorPoint(new QVector(0.5f,0.5f));
//		layer.addChildNode(videoTrack.getGraphic());
//		layer.addChildNode(duplicatenodeL);
//		layer.addChildNode(duplicatenodeR);
		combiner.getRootNode().addEffect(effect);
		combiner.getRootNode().addChildNode(composeNode);
		combiner.getRootNode().setEnable3d(true);
//		combiner.getRootNode().setBkColor(new QVector(1,0,1,1));

		QNodeAnimator an1 = new QNodeAnimator(QNodeAnimator.rotationxyz, new QRange(0, 5000) ,
				new QVector(0, 0, 0) , new QVector(-180, 180, 180), QNodeAnimator.Bounce_EaseOut ,false, "");
		QNodeAnimator an2 = new QNodeAnimator(QNodeAnimator.rotationxyz, new QRange(5000, 15000) ,
				new QVector(-180, 180, 180) , new QVector(-360, 360, 360), QNodeAnimator.Elastic_EaseOut ,false, "");
		composeNode.addAnimator(an1);
		composeNode.addAnimator(an2);
	}

	void setImagesPreview(QCombiner combiner , String[] images_path) {
		combiner.getRootNode().setBkColor(new QVector(0,0,1,1));

		QMediaTrack audioTrack = combiner.createAudioTrack("LR.mp3", true);

		int time_position = 0;
		int image_duration = 3000;

		QNodeAnimator []animator_list1 = {
				new QNodeAnimator(QNodeAnimator.rotationxyz,
						new QRange(0,image_duration),new QVector(0,0,0),new QVector(0,0,360),QNodeAnimator.Linear,false, ""),
				new QNodeAnimator(QNodeAnimator.scalexy,
						new QRange(0,image_duration),new QVector(1,1),new QVector(1.5f,1.5f),QNodeAnimator.Quart_EaseIn,false, ""),
				new QNodeAnimator(QNodeAnimator.alpha,
						new QRange(0,image_duration),new QVector(1),new QVector(0.5f),QNodeAnimator.Quart_EaseIn,false, "")
		};

		QNodeAnimator []animator_list2 = {
				new QNodeAnimator(QNodeAnimator.rotationxyz,
						new QRange(0,image_duration/2),new QVector(0,0,260),new QVector(0,0,0),QNodeAnimator.Linear,false, ""),
				new QNodeAnimator(QNodeAnimator.scalexy,
						new QRange(0,image_duration/2),new QVector(1.5f,1.5f),new QVector(1,1),QNodeAnimator.Quart_EaseOut,false, ""),
				new QNodeAnimator(QNodeAnimator.alpha,
						new QRange(0,image_duration/2),new QVector(0.5f),new QVector(1),QNodeAnimator.Quart_EaseOut,false, ""),
				new QNodeAnimator(QNodeAnimator.contentsize,
						new QRange(image_duration/2,image_duration),new QVector(targetW,targetH),new QVector(targetW*2,targetH*2),QNodeAnimator.Linear,false, "")
		};

		QNodeAnimator []animator_list3 = {
				new QNodeAnimator(QNodeAnimator.contentsize,
						new QRange(0,image_duration/2),new QVector(targetW*2,targetH*2),new QVector(targetW,targetH),QNodeAnimator.Quart_EaseOut,false, ""),
				new QNodeAnimator(QNodeAnimator.rotationxyz,
						new QRange(image_duration/2,image_duration),new QVector(0,0,0),new QVector(0,90,0),QNodeAnimator.Linear,false, "")
		};

		QNodeAnimator []animator_list4 = {
				new QNodeAnimator(QNodeAnimator.rotationxyz,
						new QRange(0,image_duration/2),new QVector(0,270,0),new QVector(0,360,0),QNodeAnimator.Linear,false, ""),

				new QNodeAnimator(QNodeAnimator.crop,
						new QRange(image_duration/2,image_duration),new QVector(0,0,1,1),new QVector(0.25f,0.25f,0.5f,0.5f),QNodeAnimator.Quart_EaseIn,false, "")
		};

		Vector<QNodeAnimator []> animators_goup_list = new Vector<>();
		animators_goup_list.add(animator_list1);
		animators_goup_list.add(animator_list2);
		animators_goup_list.add(animator_list3);
		animators_goup_list.add(animator_list4);

		int index = 0;

		for (String imagepath : images_path){
			QImageNode imageNode = new QImageNode(imagepath, false, combiner);
			imageNode.setRenderRange(new QRange(time_position, time_position + image_duration));
			imageNode.setAnchorPoint(new QVector(0.5f,0.5f));
			time_position += image_duration;

			if (index < animators_goup_list.size()){
				for (QNodeAnimator animator : animators_goup_list.get(index))
					imageNode.addAnimator(animator);
			}

			imageNode.setContentSize(new QSize(targetW, targetH));
			combiner.getRootNode().addChildNode(imageNode);

			index ++;
		}

		combiner.setValidTimeRange(new QRange(0, time_position));
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_reader_muxer);
//		Log.i(TAG, "qmediasdk version: " + QMediaSDK.SDK_VERSION);

		String lunch_mode = getIntent().getStringExtra("LAUNCH_MODE");

		media_paths = getIntent().getStringArrayExtra("MEDIA_PATHS");

		QMediaSDK.init(this);
		mframeLayout = (FrameLayout) findViewById(R.id.frameLayout);
		mPreviewView = (QPlayerView) findViewById(R.id.render_view);
		mPreviewView.setDisplayMode(QPlayerView.DisplayAdaptive);

		Log.e(TAG, "activity_create width=" + mPreviewView.getWidth() + " height=" + mPreviewView.getHeight() + " " + QMediaSDK.SDK_VERSION);

		Button btn_start = (Button) findViewById(R.id.btn_start);
		Button btn_stop = (Button) findViewById(R.id.btn_stop);
		Button btn_save = (Button) findViewById(R.id.btn_save);
		Button btn_capture = (Button) findViewById(R.id.btn_capture);

		btn_start.setOnClickListener(this);
		btn_stop.setOnClickListener(this);
		btn_save.setOnClickListener(this);
		btn_capture.setOnClickListener(this);

		mProgressBar = (TextView) findViewById(R.id.bar_progress);
//		mProgressBar.setMax(100);
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
		if (lunch_mode != null && lunch_mode.equals("SELECT_IMAGES")){
			setImagesPreview(editorPlayer, media_paths);
		}else {
			if (media_paths != null && media_paths.length > 0)
				setVideoPreview(editorPlayer, media_paths[0]);
			else
				setVideoPreview(editorPlayer, null);
		}
//		editorPlayer.setValidTimeRange(new QRange(0, 10000));
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

				final String savePath = "/sdcard/output.mp4";
				File f = new File(savePath);
				if(f.exists()){
					f.delete();
				}

				exporter = new QFileExporter(savePath);
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
						QRange range = exporter.getMediaTimeRange();
//						mProgressBar.setProgress((int) (exporter.getPosition() * 100 / range.getLength()));
						mProgressBar.setText((int) (exporter.getPosition() * 100 / range.getLength()) + "%");
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
						Toast.makeText(PreviewMuxerActivity.this, "onExporterCompleted Save Path: " + savePath, Toast.LENGTH_SHORT).show();
					}
				});
//				setEffects(exporter);
				exporter.copyForm(editorPlayer);
				exporter.start();


				mProgressBar.setVisibility(View.VISIBLE);
//				mProgressBar.setMax(100);
//				mProgressBar.setIndeterminate(false);
			}
			break;
			case R.id.btn_capture:
				break;
		}
	}
}
