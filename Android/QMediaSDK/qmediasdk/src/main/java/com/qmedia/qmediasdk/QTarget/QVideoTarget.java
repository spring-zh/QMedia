package com.qmedia.qmediasdk.QTarget;

import com.qmedia.qmediasdk.QSource.QVideoDescribe;

public interface QVideoTarget {
    boolean startVideo();
    boolean stopVideo();
    void pauseVideo(boolean isPause);
    void flushVideo();

    boolean initVideo(QVideoDescribe describe);
    int getTargetWidth();
    int getTargetHeight();
    float getFrameRate();
    Object getTargetContext();


    //render
    void setVideoRender(QVideoRender videoRender);
    QVideoRender getVideoRender();
}
