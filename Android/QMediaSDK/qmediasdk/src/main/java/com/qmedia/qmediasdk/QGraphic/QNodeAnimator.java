package com.qmedia.qmediasdk.QGraphic;

import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QCommon.QVector;

public class QNodeAnimator {

    //@property
    public final String positionxyz = "positionxyz";//v3
    public final String rotationxyz = "rotationxyz";//v3
    public final String scalex = "scalex";//v1
    public final String scalez = "scalez";//v1
    public final String scalexy = "scalexy";//v2
    public final String scalexyz = "scalexyz";//v3
    public final String contentsizew = "contentsizew";//v1
    public final String contentsizeh = "contentsizeh";//v1
    public final String contentsize = "contentsize";//v2
    public final String alpha = "alpha";//v1
    public final String mixcolor = "mixcolor";//v4

    //@EaseFunction
    public final int CUSTOM_EASING = -1;
    public final int Linear = 0;

    public final int Sine_EaseIn = 1;
    public final int Sine_EaseOut = 2;
    public final int Sine_EaseInOut = 3;

    public final int Quad_EaseIn = 4;
    public final int Quad_EaseOut = 5;
    public final int Quad_EaseInOut = 6;

    public final int Cubic_EaseIn = 7;
    public final int Cubic_EaseOut = 8;
    public final int Cubic_EaseInOut = 9;

    public final int Quart_EaseIn  = 10;
    public final int Quart_EaseOut = 11;
    public final int Quart_EaseInOut = 12;

    public final int Quint_EaseIn  = 13;
    public final int Quint_EaseOut = 14;
    public final int Quint_EaseInOut  = 15;

    public final int Expo_EaseIn = 16;
    public final int Expo_EaseOut = 17;
    public final int Expo_EaseInOut  = 18;

    public final int Circ_EaseIn = 19;
    public final int Circ_EaseOut = 20;
    public final int Circ_EaseInOut = 21;

    public final int Elastic_EaseIn = 22;
    public final int Elastic_EaseOut = 23;
    public final int Elastic_EaseInOut = 24;

    public final int Back_EaseIn = 25;
    public final int Back_EaseOut = 26;
    public final int Back_EaseInOut = 27;

    public final int Bounce_EaseIn  = 28;
    public final int Bounce_EaseOut = 29;
    public final int Bounce_EaseInOut = 30;

    public QNodeAnimator(String property, QRange timeRang, QVector beginPoint, QVector endPoint,
                         int functionType, boolean repleat ,String name ) {
        this.property = property;
        this.timeRang = timeRang;
        this.beginPoint = beginPoint;
        this.endPoint = endPoint;
        this.functionType = functionType;
        this.repleat = repleat;
        this.name = name;

        this.mPtr = native_create(property, timeRang, beginPoint, endPoint, functionType, repleat , name);
    }

    public void release(){
        native_release();
    }

    public String property;
    public QRange timeRang;
    public QVector beginPoint;
    public QVector endPoint;
    public int functionType;
    public String name;
    public boolean repleat;

    //naitve
    protected native long native_create(String property, QRange timeRang, QVector beginPoint, QVector endPoint,
                                        int functionType, boolean repleat ,String name);
    protected native void native_release();
    //native ptr
    private long mPtr = 0;
}
