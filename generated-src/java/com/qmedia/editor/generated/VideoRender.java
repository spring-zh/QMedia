// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from video_runloop.djinni

package com.qmedia.editor.generated;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import java.util.concurrent.atomic.AtomicBoolean;

public interface VideoRender {
    public void setDisplayMode(int mode, boolean flipV);

    public void onViewSizeChange(int width, int height);

    public void onViewDestroy();

    public boolean onDraw(long pirv, boolean noDisplay);

    public void readRGBA(@NonNull Object buffer, int width, int height, int format);

    static final class CppProxy implements VideoRender
    {
        private final long nativeRef;
        private final AtomicBoolean destroyed = new AtomicBoolean(false);

        private CppProxy(long nativeRef)
        {
            if (nativeRef == 0) throw new RuntimeException("nativeRef is zero");
            this.nativeRef = nativeRef;
        }

        private native void nativeDestroy(long nativeRef);
        public void _djinni_private_destroy()
        {
            boolean destroyed = this.destroyed.getAndSet(true);
            if (!destroyed) nativeDestroy(this.nativeRef);
        }
        protected void finalize() throws java.lang.Throwable
        {
            _djinni_private_destroy();
            super.finalize();
        }

        @Override
        public void setDisplayMode(int mode, boolean flipV)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_setDisplayMode(this.nativeRef, mode, flipV);
        }
        private native void native_setDisplayMode(long _nativeRef, int mode, boolean flipV);

        @Override
        public void onViewSizeChange(int width, int height)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_onViewSizeChange(this.nativeRef, width, height);
        }
        private native void native_onViewSizeChange(long _nativeRef, int width, int height);

        @Override
        public void onViewDestroy()
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_onViewDestroy(this.nativeRef);
        }
        private native void native_onViewDestroy(long _nativeRef);

        @Override
        public boolean onDraw(long pirv, boolean noDisplay)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            return native_onDraw(this.nativeRef, pirv, noDisplay);
        }
        private native boolean native_onDraw(long _nativeRef, long pirv, boolean noDisplay);

        @Override
        public void readRGBA(Object buffer, int width, int height, int format)
        {
            assert !this.destroyed.get() : "trying to use a destroyed object";
            native_readRGBA(this.nativeRef, buffer, width, height, format);
        }
        private native void native_readRGBA(long _nativeRef, Object buffer, int width, int height, int format);
    }
}
