package com.qmedia.qmediasdk.QEffect;

import com.qmedia.qmediasdk.QCommon.QRange;

public class QEffect {

    public String getName() {
        return native_getName();
    }
    public String getDescribe() {
        return native_getDescribe();
    }
    public String getType() {
        return native_getType();
    }

    public QRange getRenderRange() {
        return native_getRenderRange();
    }

    public void setRenderRange(QRange renderRange) {
        this.renderRange = renderRange;
        native_setRenderRange(renderRange);
    }

    /**
     * set key's value
     */
    public void setIntValue(String key, int value) {
        native_setIntValue(key,value);
    }
    public void setIntArrayValue(String key, int[] value, int count) {
        native_setIntArrayValue(key, value, count);
    }
    public void setFloatValue(String key, float value) {
        native_setFloatValue(key, value);
    }
    public void setFloatArrayValue(String key, float[] value, int count) {
        native_setFloatArrayValue(key, value, count);
    }

    //private
    QEffect(long native_ptr) {
        mPtr = native_ptr;
    }

    @Override
    protected void finalize() throws Throwable {
        native_release();
        mPtr = 0;
        super.finalize();
    }

    private QRange renderRange = new QRange(0,0);

    //naitve
//    protected native long native_create();
    protected native void native_release();
    protected native String native_getName();
    protected native String native_getDescribe();
    protected native String native_getType();
    protected native QRange native_getRenderRange();
    protected native void native_setRenderRange(QRange renderRange);
    protected native void native_setIntValue(String key, int value);
    protected native void native_setIntArrayValue(String key, int[] value, int count);
    protected native void native_setFloatValue(String key, float value);
    protected native void native_setFloatArrayValue(String key, float[] value, int count);
    //native ptr
    protected long mPtr = 0;
}
