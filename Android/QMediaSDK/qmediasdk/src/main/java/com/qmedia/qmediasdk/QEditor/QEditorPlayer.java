package com.qmedia.qmediasdk.QEditor;

import android.os.Looper;

import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QGraphic.QGraphicNode;
import com.qmedia.qmediasdk.QTarget.Implements.QAudioPlayer;
import com.qmedia.qmediasdk.QTarget.QAudioRender;
import com.qmedia.qmediasdk.QTarget.QVideoRender;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

import android.os.Handler;

import java.nio.ByteBuffer;

public class QEditorPlayer extends QCombiner implements QVideoRender, QAudioRender{

    //PlayerState
    public static final int State_Idle = 0;
    public static final int State_Initialized = 1;
    public static final int State_AsyncPreparing = 2;
    public static final int State_Prepared = 3;
    public static final int State_Started = 4;
    public static final int State_Paused = 5;
    public static final int State_Completed = 5;
    public static final int State_Stopped= 5;
    public static final int State_Error = 5;
    public static final int State_Ended = 5;

    public interface Observer {
        void onPrepare();
        void onPlayerStateChanged(int newState, int oldState);
        void onPlayerTimeProgressUpdated(long progress);
        void onSeekTo(long mSec);
        void onCompleted(int errcode);
    }

    public QEditorPlayer() {
        this(null);
    }

    public QEditorPlayer(Handler cbHandler) {
        mPtr = native_create(rootNode);
        QAudioPlayer audioPlayer = new QAudioPlayer();
        audioPlayer.setAudioRender(this);
        setAudioTarget(audioPlayer); // set default audio player
        if (cbHandler == null)
            mCbHandler = new Handler(Looper.getMainLooper());
        else
            mCbHandler = cbHandler;
    }

    private void init() {
        mPtr = native_create(rootNode);
        QAudioPlayer audioPlayer = new QAudioPlayer();
        audioPlayer.setAudioRender(this);
        setAudioTarget(audioPlayer); // set default audio player
    }

    public void setPlayerView(QVideoTarget playerView) {
        playerView.setVideoRender(this);
        setVideoTarget(playerView);
    }

    public void setObserver(Observer observer) {
        mObserver = observer;
    }

    //Player control
    public void start() {
        native_start();
    }
    public void play() {
        native_play();
    }
    public void pause() {
        native_pause();
    }
    public void stop() {
        native_stop();
    }
    public void seekTo(long timePoint ,int flags) {
        native_seekTo(timePoint,flags);
    }

    public int getPlayerState() {
        return native_getPlayerState();
    }

    public boolean isPaused() {
        return native_isPaused();
    }

    public void release() {
        native_release();
        super.native_target_release();
        mObserver = null;
    }

    @Override
    public boolean onAudioRender(byte[] buffer, int needBytes, long wantTime) {
        return native_onAudioRender(buffer, needBytes, wantTime);
    }

    @Override
    public boolean onVideoRender(long wantTime) {
        return native_onVideoRender(wantTime);
    }

    @Override
    public boolean onVideoCreate() {
        return native_onVideoCreate();
    }

    @Override
    public void onVideoDestroy() {
        native_onVideoDestroy();
    }

    //    private int playerState;//QEditorPlayerState
//    private boolean isPaused;
    private Handler mCbHandler;
    private Observer mObserver;

    //TODO: native
    private native long native_create(DisplayRootNode rootNode);
    private native int native_getPlayerState();
    private native boolean native_isPaused();
    private native void native_start();
    private native void native_play();
    private native void native_pause();
    private native void native_stop();
    private native void native_seekTo(long timePoint ,int flags);

    private native boolean native_onAudioRender(byte[] buffer, int needBytes, long wantTime);
    private native boolean native_onVideoRender(long wantTime) ;
    private native boolean native_onVideoCreate();
    private native boolean native_onVideoDestroy();

    private native void native_release();

    //TODO: Player callback @callbynative
    private void onPrepare(int errcode) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onPrepare();
                }
            });
        }
    }

    private void onPlayerStateChanged(int newState, int oldState) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onPlayerStateChanged(newState, oldState);
                }
            });
        }
    }

    private void onPlayerTimeProgressUpdated(long progress) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onPlayerTimeProgressUpdated(progress);
                }
            });
        }
    }

    private void onSeekTo(long mSec) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onSeekTo(mSec);
                }
            });
        }
    }

    private void onCompleted(int errcode) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onCompleted(errcode);
                }
            });
        }
    }
}
