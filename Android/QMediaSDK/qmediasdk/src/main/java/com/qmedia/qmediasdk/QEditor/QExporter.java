package com.qmedia.qmediasdk.QEditor;

import android.os.Handler;
import android.os.Looper;

import com.qmedia.qmediasdk.QTarget.QAudioRender;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;
import com.qmedia.qmediasdk.QTarget.QVideoRender;
import com.qmedia.qmediasdk.QTarget.QVideoTarget;

public class QExporter extends QCombiner implements QVideoRender, QAudioRender {

    public interface Observer {
        void onExporterStarted(int code);
        void onExporterStoped(int code);
        void onExporterProgressUpdated(long progress);
        void onExporterCanceled(int code);
        void onExporterCompleted();
    }

    private Handler mCbHandler;
    private Observer mObserver;

    public QExporter(QVideoTarget videoTarget, QAudioTarget audioTarget) {
        this(videoTarget, audioTarget, null);
    }

    public QExporter(QVideoTarget videoTarget, QAudioTarget audioTarget, Handler cbHandler) {
        mPtr = native_create(rootNode);
        videoTarget.setVideoRender(this);
        audioTarget.setAudioRender(this);
        setVideoTarget(videoTarget);
        setAudioTarget(audioTarget);

        if (cbHandler == null)
            mCbHandler = new Handler(Looper.getMainLooper());
        else
            mCbHandler = cbHandler;
    }

    public void setObserver(Observer observer) {
        mObserver = observer;
    }

    public void start() {
        native_start();
    }
    public void stop() {
        native_stop();
    }
    public void cancel() {
        native_cancel();
    }
    public void release() {
        native_release();
        super.native_target_release();
        mObserver = null;
    }

    public DisplayRootNode getRootNode() {
        return rootNode;
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

    //TODO: native
    private native long native_create(DisplayRootNode rootNode);
    private native void native_start();
    private native void native_stop();
    private native void native_cancel();

    private native boolean native_onAudioRender(byte[] buffer, int needBytes, long wantTime);
    private native boolean native_onVideoRender(long wantTime) ;
    private native boolean native_onVideoCreate();
    private native boolean native_onVideoDestroy();

    private native void native_release();

    //TODO: QExporter callback @callbynative
    private void onExporterStarted(int code) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onExporterStarted(code);
                }
            });
        }
    }
    private void onExporterStoped(int code) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onExporterStoped(code);
                }
            });
        }
    }
    private void onExporterProgressUpdated(long progress) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onExporterProgressUpdated(progress);
                }
            });
        }
    }
    private void onExporterCanceled(int code) {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onExporterCanceled(code);
                }
            });
        }
    }
    private void onExporterCompleted() {
        if (mObserver != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    mObserver.onExporterCompleted();
                }
            });
        }
    }
}
