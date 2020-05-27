package com.qmedia.qmediasdk.QGraphic;

import com.qmedia.qmediasdk.QCommon.QRange;
import com.qmedia.qmediasdk.QCommon.QVector;
import com.qmedia.qmediasdk.QEditor.QCombiner;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

public class QGraphicNode {
    private static final String TAG = "QGraphicNode";
    public QGraphicNode(String name, QCombiner combiner) {
        this(combiner);
        mPtr = native_create();
        setName(name);
    }

    //TODO: construct this for child class
    protected QGraphicNode(QCombiner combiner) {
        weakCombiner = new WeakReference<>(combiner);
        combiner.addGraphicNode(this);
    }

    //TODO: childrens @QGraphicNode
    public ArrayList<QGraphicNode> getChildrens() {
        return childrens;
    }
    public boolean addChildNode(QGraphicNode child) {
        if (child != null && !childrens.contains(child)){
            childrens.add(child);
            child.parent = this;
            weakCombiner.get().attachRenderNode(child, this);
            return true;
        }
        return false;
    }
    public boolean removeChildNode(QGraphicNode child) {
        if (childrens.contains(child)) {
            child.parent = null;
            childrens.remove(child);
            weakCombiner.get().detachRenderNode(child);
            return true;
        }
        return false;
    }

    public QGraphicNode getParent() {
        return parent;
    }

    //TODO: animators @QNodeAnimator
    public ArrayList<QNodeAnimator> getAnimators() {
        return animators;
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

    public String getName() {
        return native_getName();
    }

    public void setName(String name) {
        this.name = name;
        native_setName(name);
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
        childrens.clear();
        animators.clear();
    }


    private WeakReference<QCombiner> weakCombiner = null;
    private QGraphicNode parent = null;
    private ArrayList<QGraphicNode> childrens = new ArrayList();
    private ArrayList<QNodeAnimator> animators = new ArrayList();

    String name = "";
    QRange renderRange = new QRange(0,0);
    //transform propertys setting
    boolean visible = false;

    float rotation = 0;
    QVector rotation3d = new QVector(0,0,0);
    float scaleX = 1;
    float scaleY = 1;
    float scaleZ = 1;

    QVector contentSize = new QVector(0,0);
    QVector position = new QVector(0,0);;
    float positionZ = 0;

    QVector anchorPoint = new QVector(0,0);;
    QVector color4 = new QVector(1,1,1,1);;
    float alpha = 1;


    //naitve
    protected native long native_create();
    protected native boolean native_addAnimator(QNodeAnimator animator);
    protected native boolean native_removeAnimator(QNodeAnimator animator);

    protected native String native_getName();
    protected native void native_setName(String name);

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
    protected long mPtr = 0;
}
