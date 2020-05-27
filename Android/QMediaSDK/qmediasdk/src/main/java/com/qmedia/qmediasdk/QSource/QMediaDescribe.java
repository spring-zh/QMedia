package com.qmedia.qmediasdk.QSource;

public class QMediaDescribe {
    //enum QMediaType
    public static final int QMediaTypeVideo = 1;
    public static final int QMediaTypeAudio = 2;
    public static final int QMediaTypeSubtitle = 4;

    public int mediaType; //QMediaType
    public QVideoDescribe videoDescribe = null;
    public QAudioDescribe audioDescribe = null;
}
