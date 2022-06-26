// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from audio_runloop.djinni

package com.qmedia.editor.generated;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public interface AudioRunloop {
    public void setAudioRender(@Nullable AudioRender render);

    public boolean audioInit(@NonNull AudioDescription desc);

    public boolean audioStart();

    public void audioStop();

    public void audioPause(boolean bpause);

    public void audioFlush();

    public void setVolume(float volume);

    public float getVolume();
}