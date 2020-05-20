package com.qmedia.qmediasdk.QTarget;

import java.nio.ByteBuffer;

public interface QAudioRender {
    boolean onAudioRender(ByteBuffer buffer , int needBytes , long wantTime);
}
