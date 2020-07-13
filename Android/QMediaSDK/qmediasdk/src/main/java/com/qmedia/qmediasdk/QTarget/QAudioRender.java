package com.qmedia.qmediasdk.QTarget;

public interface QAudioRender {
    boolean onAudioRender(byte[] buffer , int needBytes , long wantTime);
}
