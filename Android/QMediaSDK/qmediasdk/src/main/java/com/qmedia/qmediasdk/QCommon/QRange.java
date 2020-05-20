package com.qmedia.qmediasdk.QCommon;

public class QRange {
    public QRange(){
        this.start = 0;
        this.end = 0;
    }
    public QRange(long start, long end){
        this.start = start;
        this.end = end;
    }

    public boolean isValid() {
        return end > start;
    }

    public long start;
    public long end;
}
