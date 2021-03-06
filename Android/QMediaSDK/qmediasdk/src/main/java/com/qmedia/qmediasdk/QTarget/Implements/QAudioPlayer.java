package com.qmedia.qmediasdk.QTarget.Implements;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.PlaybackParams;
import android.os.Build;
import android.os.Process;
import android.util.Log;

import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QTarget.QAudioRender;
import com.qmedia.qmediasdk.QTarget.QAudioTarget;

import java.nio.ByteBuffer;

public class QAudioPlayer implements QAudioTarget , Runnable{
    private final static String TAG = "QAudioPlayer";
    private QAudioRender audioRender;
    int audioDelay = 0;
    private QAudioDescribe describe;
    private final int AUDIOTRACK_PLAYBACK_MAXSPEED = 2;

    //AudioTrack
    private AudioTrack atrack;
    private ByteBuffer audioData;
    private Thread playThread;
    private boolean abort_request = true;
    private boolean pause_on = true;
    private boolean need_flush = false;
    private boolean need_set_volume = false;
    private float left_volume = 1.0f;
    private float right_volume = 1.0f;
    private boolean speed_changed = false;
    private float speed = 1.0f;
    private Object wakeup = new Object();

    private boolean isInit = false;
    private boolean isStarted = false;

    @Override
    public void run() {
        final int copy_size = 512;
        assert (audioData.capacity() > copy_size);
        assert (atrack != null);

        android.os.Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);

        if (!abort_request && !pause_on)
            atrack.play();

        while (!abort_request) {
            try {
                synchronized (wakeup) {
                    if (!abort_request && pause_on) {
                        atrack.pause();
                        while (!abort_request && pause_on) {
                            wakeup.wait(1);
                        }
                        if (!abort_request && !pause_on) {
                            if (need_flush) {
                                need_flush = false;
                                atrack.flush();
                            }
                            atrack.play();
                        }
                    }
                    if (need_flush) {
                        need_flush = false;
                        atrack.flush();
                    }
                    if (need_set_volume) {
                        need_set_volume = false;
                        atrack.setStereoVolume(left_volume, right_volume);
                    }
                    if (speed_changed) {
                        speed_changed = false;
                        setSpeed(speed);
                    }
                }

                audioRender.onAudioRender(audioData.array(), copy_size, -1);
                if (need_flush){
                    need_flush = false;
                    atrack.flush();
                }else {
                    int written = atrack.write(audioData.array(), 0, copy_size);
                    if (written != copy_size) {
                        Log.w(TAG, "AudioTrack: not all data copied" + written + "/" + copy_size);
                    }
                }
            }catch (InterruptedException e) {
                e.printStackTrace();
            }

        }

        atrack.stop();
    }

    @Override
    public boolean startAudio() {
        Log.d(TAG, "startAudio");
        if (! isInit ) return false;
        pause_on = false;
        abort_request = false;
        isStarted = true;
        playThread = new Thread(this, "QAudioPlayer");
        playThread.start();
        return true;
    }

    @Override
    public boolean stopAudio() {
        Log.d(TAG, "stopAudio");
        if (isStarted) {
            synchronized (wakeup) {
                abort_request = true;
                wakeup.notify();
            }

            if (playThread != null) {
                try {
                    playThread.join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                playThread = null;
            }
        }
        isStarted = false;
        return true;
    }

    @Override
    public void pauseAudio() {
        Log.d(TAG, "pauseAudio");
        synchronized (wakeup) {
            pause_on = true;
            wakeup.notify();
        }
    }

    @Override
    public void resumeAudio() {
        Log.d(TAG, "resumeAudio");
        synchronized (wakeup) {
            pause_on = false;
            wakeup.notify();
        }
    }

    @Override
    public void flushAudio() {
        Log.d(TAG, "flushAudio");
        synchronized (wakeup) {
            need_flush = true;
            wakeup.notify();
        }
    }

    @Override
    public boolean initAudio(QAudioDescribe describe) {
        Log.d(TAG, "initAudio samplerate:" + describe.samplerate + "channel:" + describe.nchannel);
        if (isInit) return true;

        this.describe = describe;
        int channelout = AudioFormat.CHANNEL_OUT_STEREO;
        if (describe.nchannel == 1)
            channelout =  AudioFormat.CHANNEL_OUT_MONO;
        int pcm_bits = AudioFormat.ENCODING_PCM_16BIT;
        switch (describe.rawFormat){
            case QAudioDescribe.QRawAudioFormatU8:
                pcm_bits = AudioFormat.ENCODING_PCM_8BIT;
                break;
            case QAudioDescribe.QRawAudioFormatS16:
                pcm_bits = AudioFormat.ENCODING_PCM_16BIT;
                break;
            case QAudioDescribe.QRawAudioFormatFLOAT:
                pcm_bits = AudioFormat.ENCODING_PCM_FLOAT;
                break;
            default:
                break;
        }
        int minBufferSize = AudioTrack.getMinBufferSize(describe.samplerate, channelout, pcm_bits);
        if (minBufferSize <= 0) {
            Log.e(TAG, "AudioTrack.getMinBufferSize < 0 ");
            return false;
        }
        Log.i(TAG, "AudioTrack.getMinBufferSize " + minBufferSize);
        minBufferSize *= AUDIOTRACK_PLAYBACK_MAXSPEED;
        audioData = ByteBuffer.allocate(minBufferSize);

        atrack = new AudioTrack(AudioManager.STREAM_MUSIC, describe.samplerate, channelout, pcm_bits, minBufferSize, AudioTrack.MODE_STREAM);
        isInit = true;
        return true;
    }

    @Override
    public void unInitAudio() {
        if (isInit) {
            atrack.release();
            atrack = null;
            audioData = null;
        }
        isInit = false;
    }

    @Override
    public int getSampleRate() {
        return describe.samplerate;
    }

    @Override
    public int getChannels() {
        return describe.nchannel;
    }

    @Override
    public int getFormat() {
        return describe.rawFormat;
    }

    @Override
    public void setVolume(float volume) {
        Log.d(TAG, "setVolume: " + volume);
        synchronized (wakeup) {
            this.right_volume = volume;
            this.left_volume = volume;
            need_set_volume = true;
            wakeup.notify();
        }
    }

    @Override
    public float getVolume() {
        return right_volume;
    }

    @Override
    public void setAudioDelay(int audioDelay) {
        this.audioDelay = audioDelay;
    }

    @Override
    public int getAudioDelay() {
        return audioDelay;
    }

    @Override
    public void setAudioRender(QAudioRender audioRender) {
        this.audioRender = audioRender;
    }

    @Override
    public QAudioRender getAudioRender() {
        return audioRender;
    }

    private void setSpeed(float speed) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            int sample_rate = atrack.getSampleRate();
            atrack.setPlaybackRate((int) (sample_rate * speed));
            return;
        }

        PlaybackParams params = atrack.getPlaybackParams();
        params.setSpeed(speed);
        atrack.setPlaybackParams(params);
    }
}
