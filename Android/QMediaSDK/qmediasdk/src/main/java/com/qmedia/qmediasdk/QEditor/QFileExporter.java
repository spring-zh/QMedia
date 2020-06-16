package com.qmedia.qmediasdk.QEditor;

import android.os.Handler;

import com.qmedia.qmediasdk.QTarget.Implements.QFileExporterTarget;

public class QFileExporter extends QExporter {
    public QFileExporter(String filePath) {
        this(filePath, null);
    }

    public QFileExporter(String filePath, Handler cbHandler) {
        super(cbHandler);
        this.filePath = filePath;
        fileExporterTarget = new QFileExporterTarget(filePath);
        setVideoTarget(fileExporterTarget);
        setAudioTarget(fileExporterTarget);
        fileExporterTarget.setVideoRender(this);
        fileExporterTarget.setAudioRender(this);
    }

    public String getFilePath() {
        return filePath;
    }

    private QFileExporterTarget fileExporterTarget;
    private String filePath;
}
