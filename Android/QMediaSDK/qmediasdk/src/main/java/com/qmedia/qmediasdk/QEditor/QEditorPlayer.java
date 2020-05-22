package com.qmedia.qmediasdk.QEditor;

import android.os.Looper;

import com.qmedia.qmediasdk.QTarget.Implements.QAudioPlayer;
import com.qmedia.qmediasdk.QTarget.QAudioRender;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoRender;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

import android.os.Handler;

import java.nio.ByteBuffer;

public class QEditorPlayer extends QCombiner implements QVideoRender, QAudioRender{

    public interface Observer {
        void onPrepare();
        void onPlayerStateChanged(int newState, int oldState);
        void onPlayerTimeProgressUpdated(long progress);
        void onSeekTo(long mSec);
        void onCompleted(int errcode);
    }

//    public enum State {
//        Idle(0),
//        Initialized(1),
//        AsyncPreparing(2),
//        Prepared(3),
//        Started(4),
//        Paused(5),
//        Completed(6),
//        Stopped(7),
//        Error(8),
//        Ended(9);
//
//        State(int value){
//            this.value = value;
//        }
//
//        public int getValue() {
//            return value;
//        }
//
//        public void setValue(int value) {
//            this.value = value;
//        }
//
//        private int value;
//    }

    public QEditorPlayer() {
        mCbHandler = new Handler(Looper.getMainLooper());
        init();
    }

    public QEditorPlayer(Handler cbHandler) {
        mCbHandler = cbHandler;
        init();
    }

    private void init() {
        native_create();
        QAudioTarget audioTarget = new QAudioPlayer();
        mediaFactory.setAudioTarget(audioTarget, this);
    }

    public void setPlayerView(QVideoTarget playerView) {
        mediaFactory.setVideoTarget(playerView, this);
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
        return getPlayerState();
    }

    public boolean isPaused() {
        return native_isPaused();
    }

    public void release() {
        native_release();
        mediaFactory.release();
        mObserver = null;
    }

    @Override
    public boolean onAudioRender(ByteBuffer buffer, int needBytes, long wantTime) {
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
    private native long native_create();
    private native int native_getPlayerState();
    private native boolean native_isPaused();
    private native void native_start();
    private native void native_play();
    private native void native_pause();
    private native void native_stop();
    private native void native_seekTo(long timePoint ,int flags);

    private native boolean native_onAudioRender(ByteBuffer buffer, int needBytes, long wantTime);
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
