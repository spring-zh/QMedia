package com.qmedia.qmediasdk.QTarget;

import java.nio.ByteBuffer;

public interface QAudioRender {
    boolean onAudioRender(byte[] buffer , int needBytes , long wantTime);
}
