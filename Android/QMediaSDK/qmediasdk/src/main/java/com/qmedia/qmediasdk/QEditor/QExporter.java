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
        rootNode.setFlipMode(2);
        videoTarget.setVideoRender(this);
        audioTarget.setAudioRender(this);
        setVideoTarget(videoTarget);
        setAudioTarget(audioTarget);

        if (cbHandler == null)
            mCbHandler = new Handler(Looper.getMainLooper());
        else
            mCbHandler = cbHandler;
    }

    //for child class
    protected QExporter(Handler cbHandler) {
        mPtr = native_create(rootNode);
        rootNode.setFlipMode(2);
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
    @Override
    public void release() {
        native_release();
        super.release();
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
    public void setDisplayMode(int displayMode, int viewW, int viewH) {
        native_setDisplayMode(displayMode, viewW, viewH);
    }

    @Override
    public void onVideoDestroy() {
        native_onVideoDestroy();
    }

    //TODO: native
    private native long native_create(QDisplayLayer rootNode);
    private native void native_start();
    private native void native_stop();
    private native void native_cancel();

    private native boolean native_onAudioRender(byte[] buffer, int needBytes, long wantTime);
    private native boolean native_onVideoRender(long wantTime) ;
    private native boolean native_onVideoCreate();
    private native boolean native_onVideoDestroy();
    private native boolean native_setDisplayMode(int displayMode, int viewW, int viewH);

    private native void native_release();

    //TODO: QExporter callback @callbynative
    private void onExporterStarted(int code) {
        Observer observer = mObserver;
        if (observer != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    observer.onExporterStarted(code);
                }
            });
        }
    }
    private void onExporterStoped(int code) {
        Observer observer = mObserver;
        if (observer != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    observer.onExporterStoped(code);
                }
            });
        }
    }
    private void onExporterProgressUpdated(long progress) {
        Observer observer = mObserver;
        if (observer != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    observer.onExporterProgressUpdated(progress);
                }
            });
        }
    }
    private void onExporterCanceled(int code) {
        Observer observer = mObserver;
        if (observer != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    observer.onExporterCanceled(code);
                }
            });
        }
    }
    private void onExporterCompleted() {
        Observer observer = mObserver;
        if (observer != null) {
            mCbHandler.post(new Runnable() {
                @Override
                public void run() {
                    observer.onExporterCompleted();
                }
            });
        }
    }
}
