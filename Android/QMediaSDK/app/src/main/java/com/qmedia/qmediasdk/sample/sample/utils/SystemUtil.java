package com.qmedia.qmediasdk.sample.sample.utils;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Point;
import android.os.Build;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.ViewConfiguration;
import android.view.WindowManager;

import java.io.File;


public class SystemUtil {

    public static float density = 1;
    public static Point displaySize = new Point();
    public static DisplayMetrics displayMetrics = new DisplayMetrics();
    public static final Handler uiHandler = new Handler(Looper.getMainLooper());
    public static int statusBarHeight = 0;

    private SystemUtil() {
    }


    public static void init(Context context) {
        density = context.getResources().getDisplayMetrics().density;
        checkDisplaySize(context);
        statusBarHeight = getStatusBarHeight();
    }

    /**
     * get root directory
     *
     * @param applicationContext
     * @return
     */
    public static File getStoreDir(Context applicationContext) {
        File dataDir = null;
        if (Environment.MEDIA_MOUNTED.equalsIgnoreCase(Environment
                .getExternalStorageState())) {
            dataDir = Environment.getExternalStorageDirectory();
        } else {
            dataDir = applicationContext.getApplicationContext().getFilesDir();
        }
        return dataDir;
    }


    public static int dp(float value) {
        if (value == 0) {
            return 0;
        }
        return (int) Math.ceil(density * value);
    }

    public static void checkDisplaySize(Context context) {
        try {
            WindowManager manager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
            if (manager != null) {
                Display display = manager.getDefaultDisplay();
                if (display != null) {
                    display.getMetrics(displayMetrics);
                    display.getSize(displaySize);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (displayMetrics != null && displayMetrics.heightPixels < displayMetrics.widthPixels) {
            final int tmp = displayMetrics.heightPixels;
            displayMetrics.heightPixels = displayMetrics.widthPixels;
            displayMetrics.widthPixels = tmp;
        }
        if (displaySize != null && displaySize.y < displaySize.x) {
            final int tmp = displaySize.y;
            displaySize.y = displaySize.x;
            displaySize.x = tmp;
        }
    }

    public static String getSdkVersion() {
        try {
            return Build.VERSION.SDK;
        } catch (Exception e) {
            e.printStackTrace();
            return String.valueOf(getSdkVersionInt());
        }
    }

    public static int getSdkVersionInt() {
        try {
            return Build.VERSION.SDK_INT;
        } catch (Exception e) {
            e.printStackTrace();
            return 0;
        }
    }

    public static void runOnUIThread(Runnable runnable) {
        runOnUIThread(runnable, 0);
    }

    public static void runOnUIThread(Runnable runnable, long delay) {
        if (delay == 0) {
            uiHandler.post(runnable);
        } else {
            uiHandler.postDelayed(runnable, delay);
        }
    }

    public static void cancelTask(Runnable runnable) {
        if (runnable != null) {
            uiHandler.removeCallbacks(runnable);
        }
    }

    public static int getStatusBarHeight() {
        return Resources.getSystem().getDimensionPixelSize(
                Resources.getSystem().getIdentifier("status_bar_height", "dimen", "android"));
    }

    public static int getNavigationBarHeight(Context context) {

        boolean hasMenuKey = ViewConfiguration.get(context).hasPermanentMenuKey();
        boolean hasBackKey = KeyCharacterMap.deviceHasKey(KeyEvent.KEYCODE_BACK);
        if (!hasMenuKey && !hasBackKey) {
            Resources resources = context.getResources();
            int resourceId = resources.getIdentifier("navigation_bar_height", "dimen", "android");
            int height = resources.getDimensionPixelSize(resourceId);
            return height;
        } else {
            return 0;
        }
    }

    public static String getForamtedTime(long ms) {
        long seconds = ms / 1000;
        long minutes = seconds / 60;
        float pseconds = seconds - minutes * 60 + (ms % 1000) / 1000.0f;
        return String.format("%02d:%2.2f", minutes, pseconds);
    }
}
