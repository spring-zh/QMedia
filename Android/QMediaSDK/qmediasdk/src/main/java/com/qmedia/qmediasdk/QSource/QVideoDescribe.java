package com.qmedia.qmediasdk.QSource;

public class QVideoDescribe {
    //enum QVideoCodec
    public static final int QVideoCodecUnknown = 0;
    public static final int QVideoCodecVP8 = 1;
    public static final int QVideoCodecVP9 = 2;
    public static final int QVideoCodecMPEG4 = 3;
    public static final int QVideoCodecH264 = 4;
    public static final int QVideoCodecH265 = 5;
    public static final int QVideoCodecI420 = 6;

    //enum QRawVideoFormat
    public static final int QRawVideoFormatI420 = 0;
    public static final int QRawVideoFormatYV12 = 1;
    public static final int QRawVideoFormatYUY2 = 2;
    public static final int QRawVideoFormatUYVY = 3;
    public static final int QRawVideoFormatIYUV = 4;
    public static final int QRawVideoFormatARGB = 5;
    public static final int QRawVideoFormatRGB24 = 6;
    public static final int QRawVideoFormatRGB565 = 7;
    public static final int QRawVideoFormatARGB4444 = 8;
    public static final int QRawVideoFormatARGB1555 = 9;
    public static final int QRawVideoFormatMJPEG = 10;
    public static final int QRawVideoFormatNV12 = 11;
    public static final int QRawVideoFormatNV21 = 12;
    public static final int QRawVideoFormatBGRA = 13;
    public static final int QRawVideoFormatHardware = 98;
    public static final int QRawVideoFormatUnknown = 99;

    //enum QVideoRotation
    public static final int QVideoRotation_0 = 0;
    public static final int QVideoRotation_90 = 90;
    public static final int QVideoRotation_180 = 180;
    public static final int QVideoRotation_270 = 270;

    public QVideoDescribe(int codec, int rawFormat, int rotation, float framerate, int width, int height, int bitrate, int timeScale){
        this.codec = codec;
        this.rawFormat = rawFormat;
        this.rotation = rotation;
        this.framerate = framerate;
        this.width = width;
        this.height = height;
        this.bitrate = bitrate;
        this.timeScale = timeScale;
    }
    public int codec; //QVideoCodec
    public int rawFormat; //QRawVideoFormat
    public int rotation; //QVideoRotation
    public float framerate;
    public int width;
    public int height;
    public int bitrate;
    public int timeScale;
}
