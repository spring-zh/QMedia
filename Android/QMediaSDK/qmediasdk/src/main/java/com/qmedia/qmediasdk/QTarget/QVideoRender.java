package com.qmedia.qmediasdk.QTarget;

public interface QVideoRender {
    boolean onVideoRender(long wantTime);
    boolean onVideoCreate();
    void onVideoDestroy();
}
