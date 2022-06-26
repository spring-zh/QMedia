// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from media_engine.djinni

package com.qmedia.editor.generated;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import java.util.concurrent.atomic.AtomicBoolean;

public interface MediaEngine {
    @NonNull
    public static String version()
    {
        return CppProxy.version();
    }

    /** set audio output paramenters */
    public static void configAudioOut(@NonNull AudioDescription desc)
    {
        CppProxy.configAudioOut(desc);
    }

    @NonNull
    public static AudioDescription audioOut()
    {
        return CppProxy.audioOut();
    }

    @Nullable
    public static EditorPlayerInternal createEditorPlayerInternal()
    {
        return CppProxy.createEditorPlayerInternal();
    }

    @Nullable
    public static EditorExporterInternal createEditorExporterInternal()
    {
        return CppProxy.createEditorExporterInternal();
    }

    static final class CppProxy implements MediaEngine
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

        @NonNull
        public static native String version();

        public static native void configAudioOut(@NonNull AudioDescription desc);

        @NonNull
        public static native AudioDescription audioOut();

        @Nullable
        public static native EditorPlayerInternal createEditorPlayerInternal();

        @Nullable
        public static native EditorExporterInternal createEditorExporterInternal();
    }
}