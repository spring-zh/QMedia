package com.qmedia.qmediasdk;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

public class QMediaSDK {

    static {
        System.loadLibrary("qmediasdk");
    }

    public static boolean init(Context context) {
        g_context = context;
        g_assetManager = context.getAssets();
        Log.i("QMediaSDK", "init sdk version: " + SDK_VERSION);
        return true;
    }

    public static Context getContext() {
        return g_context;
    }

    public static final String SDK_VERSION = _version();

    public static AssetManager g_assetManager;


    private static Context g_context;

    static private native String _version();
}
