package com.qmedia.qmediasdk.QGraphic;

import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QCommon.QVector;

import java.util.ArrayList;

public class QGraphicNode {
    public QGraphicNode(String name) {
        mPtr = native_create(name);
    }
    protected QGraphicNode() {
        //TODO: construct this for child class
    }

    public ArrayList<QGraphicNode> getChildrens() {
        return childrens;
    }
    public QGraphicNode getParent() {
        return parent;
    }

    public boolean addAnimator(QNodeAnimator animator) {
        if (animators.contains(animator))
            return true;
        animators.add(animator);
        return native_addAnimator(animator);
    }

    public boolean removeAnimator(QNodeAnimator animator) {
        if (animators.contains(animator)){
            native_removeAnimator(animator);
            animators.remove(animator);
            return true;
        }
        return false;
    }

    public QRange getRenderRange() {
        return native_getRenderRange();
    }

    public void setRenderRange(QRange renderRange) {
        this.renderRange = renderRange;
        native_setRenderRange(renderRange);
    }

    public boolean isVisible() {
        return native_isVisible();
    }

    public void setVisible(boolean visible) {
        this.visible = visible;
        native_setVisible(visible);
    }

    public QVector getRotation3d() {
        return native_getRotation3d();
    }

    public void setRotation3d(QVector rotation3d) {
        this.rotation3d = rotation3d;
        native_setRotation3d(rotation3d);
    }

    public float getRotation() {
        return native_getRotation();
    }

    public void setRotation(float rotation) {
        this.rotation = rotation;
        native_setRotation(rotation);
    }

    public float getScaleX() {
        return native_getScaleX();
    }

    public void setScaleX(float scaleX) {
        this.scaleX = scaleX;
        native_setScaleX(scaleX);
    }

    public float getScaleY() {
        return native_getScaleY();
    }

    public void setScaleY(float scaleY) {
        this.scaleY = scaleY;
        native_setScaleY(scaleY);
    }

    public float getScaleZ() {
        return native_getScaleZ();
    }

    public void setScaleZ(float scaleZ) {
        this.scaleZ = scaleZ;
        native_setScaleZ(scaleZ);
    }

    public QVector getContentSize() {
        return native_getContentSize();
    }

    public void setContentSize(QVector contentSize) {
        this.contentSize = contentSize;
        native_setContentSize(contentSize);
    }

    public QVector getPosition() {
        return native_getPosition();
    }

    public void setPosition(QVector position) {
        this.position = position;
        native_setPosition(position);
    }

    public float getPositionZ() {
        return native_getPositionZ();
    }

    public void setPositionZ(float positionZ) {
        this.positionZ = positionZ;
        native_setPositionZ(positionZ);
    }

    public QVector getAnchorPoint() {
        return native_getAnchorPoint();
    }

    public void setAnchorPoint(QVector anchorPoint) {
        this.anchorPoint = anchorPoint;
        native_setAnchorPoint(anchorPoint);
    }

    public QVector getColor4() {
        return native_getColor4();
    }

    public void setColor4(QVector color4) {
        this.color4 = color4;
        native_setColor4(color4);
    }

    public float getAlpha() {
        return native_getAlpha();
    }

    public void setAlpha(float alpha) {
        this.alpha = alpha;
        native_setAlpha(alpha);
    }

    public void release(){
        native_release();
    }


    private QGraphicNode parent = null;
    private ArrayList<QGraphicNode> childrens = new ArrayList();
    private ArrayList<QNodeAnimator> animators = new ArrayList();

    QRange renderRange;
    //transform propertys setting
    boolean visible;

    float rotation;
    QVector rotation3d;
    float scaleX;
    float scaleY;
    float scaleZ;

    QVector contentSize;
    QVector position;
    float positionZ;

    QVector anchorPoint;
    QVector color4;
    float alpha;


    //naitve
    protected native long native_create(String name);
    protected native boolean native_addAnimator(QNodeAnimator animator);
    protected native boolean native_removeAnimator(QNodeAnimator animator);

    protected native QRange native_getRenderRange();

    protected native void native_setRenderRange(QRange renderRange) ;

    protected native boolean native_isVisible() ;
    protected native void native_setVisible(boolean visible);

    protected native QVector native_getRotation3d() ;

    protected native void native_setRotation3d(QVector rotation3d) ;
    protected native  float native_getRotation() ;

    protected native  void native_setRotation(float rotation) ;

    protected native float native_getScaleX() ;

    protected native void native_setScaleX(float scaleX) ;

    protected native float native_getScaleY() ;

    protected native void native_setScaleY(float scaleY);

    protected native float native_getScaleZ() ;

    protected native void native_setScaleZ(float scaleZ);

    protected native QVector native_getContentSize() ;

    protected native void native_setContentSize(QVector contentSize) ;

    protected native QVector native_getPosition() ;

    protected native void native_setPosition(QVector position) ;

    protected native float native_getPositionZ();

    protected native void native_setPositionZ(float positionZ);

    protected native QVector native_getAnchorPoint() ;

    protected native void native_setAnchorPoint(QVector anchorPoint) ;

    protected native QVector native_getColor4() ;

    protected native void native_setColor4(QVector color4);

    protected native float native_getAlpha();

    protected native void native_setAlpha(float alpha) ;

    protected native void native_release();
    //native ptr
    private long mPtr = 0;
}
