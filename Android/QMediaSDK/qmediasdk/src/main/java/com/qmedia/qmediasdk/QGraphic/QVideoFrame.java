package com.qmedia.qmediasdk.QGraphic;

import java.nio.ByteBuffer;

public class QVideoFrame {

    //format : if is_texture is set
    public static final int TEXTURE_OES = 1;
    public static final int TEXTURE_RGBA = 2;

    public QVideoFrame(int textureid, int width, int height, int format, long timestamp) {
        is_texture = true;
        this.textureid = textureid;
        this.width = width;
        this.height = height;
        this.format = format;
        this.timestamp = timestamp;
    }

    public QVideoFrame(ByteBuffer buffer, int width, int height, int format, long timestamp) {
        is_texture = false;
        this.buffer = buffer;
        this.width = width;
        this.height = height;
        this.format = format;
        this.timestamp = timestamp;
    }

    public int width() {
        return width;
    }
    public int height() {
        return height;
    }
    public boolean is_texture() {
        return is_texture;
    }
    public ByteBuffer buffer() {
        return buffer;
    }

    public long timestamp() {
        return timestamp;
    }

    public int textureid() {
        return textureid;
    }

    public int getFormat() {
        return format;
    }

    private int width;
    private int height;
    private boolean is_texture;
    private ByteBuffer buffer;
    private int textureid;
    private int format; // if not is_texture use QRawVideoFormat

    long timestamp;
}
