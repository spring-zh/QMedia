package com.qmedia.qmediasdk.QSource;

public class QAudioDescribe {
    //enum QAudioCodec
    public static final int QAudioCodecUnknown = 0;
    public static final int QAudioCodecAAC = 1;
    public static final int QAudioCodecOpus = 2;
    public static final int QAudioCodecPcmA = 3;
    public static final int QAudioCodecPcmU  = 4;
    public static final int QAudioCodecG722 = 5;

    //enum QRawAudioFormat
    public static final int QRawAudioFormatU8 = 0;
    public static final int QRawAudioFormatS16 = 1;
    public static final int QRawAudioFormatU16 = 2;
    public static final int QRawAudioFormatS32 = 3;
    public static final int QRawAudioFormatU32 = 4;
    public static final int QRawAudioFormatFLOAT = 5;
    public static final int QRawAudioFormatUnknown = 99;

    public QAudioDescribe(int codec, int rawFormat, int samplerate, int nchannel, int bitrate) {
        this.codec = codec;
        this.rawFormat = rawFormat;
        this.samplerate = samplerate;
        this.nchannel = nchannel;
        this.bitrate = bitrate;
    }
    public int codec; //QAudioCodec
    public int rawFormat; //QRawAudioFormat
    public int samplerate;
    public int nchannel;
    public int bitrate;
}
