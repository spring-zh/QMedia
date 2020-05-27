package com.qmedia.qmediasdk;

public class QMediaSDK {

    static {
        System.loadLibrary("qmediasdk");
    }

    public static final String SDK_VERSION = _version();

    static private native String _version();
}
