package com.qmedia.qmediasdk.QTrack;

import com.qmedia.qmediasdk.QAudio.QAudioTrackNode;
import com.qmedia.qmediasdk.QEditor.QCombiner;
import com.qmedia.qmediasdk.QGraphic.QVideoTrackNode;
import com.qmedia.qmediasdk.QSource.QAudioDescribe;
import com.qmedia.qmediasdk.QSource.QVideoDescribe;

import java.util.List;
import java.util.UUID;

public class QMultiMediaTrack extends QMediaTrack {
    public QMultiMediaTrack(List<String> fileList, QCombiner combiner) {
        this(fileList, combiner, UUID.randomUUID().toString());
    }
    public QMultiMediaTrack(List<String> fileList, QCombiner combiner, String id) {
        super(id);
        _fileList = fileList;
        mPtr = native_create(fileList, combiner);
    }

    @Override
    public QVideoDescribe getVideoDescribe() {
        return native_getVideoDescribe();
    }
    @Override
    public QAudioDescribe getAudioDescribe() {
        return native_getAudioDescribe();
    }

    @Override
    public boolean generateVideoTrackNode(QCombiner combiner) {
        if (native_getVideoDescribe() != null)
            graphic = new QVideoTrackNode(this, combiner);
        return graphic != null;
    }
    @Override
    public boolean generateAudioTrackNode(QCombiner combiner) {
        if (native_getAudioDescribe() != null)
            audio = new QAudioTrackNode(this, combiner);
        return audio != null;
    }
    @Override
    public boolean generateVideoTrackNode(QCombiner combiner, String id) {
        if (native_getVideoDescribe() != null)
            graphic = new QVideoTrackNode(this, combiner, id);
        return graphic != null;
    }

    public List<String> getFileList() {
        return _fileList;
    }

    private List<String> _fileList;

    protected native long native_create(List<String> fileList, QCombiner combiner);
    protected native QVideoDescribe native_getVideoDescribe();
    protected native QAudioDescribe native_getAudioDescribe();
}
