//
//  GcNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_NODE_H
#define GRAPHICCORE_NODE_H

#include <vector>
#include <string>
#include "GcScene.h"

GRAPHICCORE_BEGIN

class Node {
public:
    Node();
    virtual ~Node() {}

    virtual void draw(Scene* /*scene*/, const Mat4 & /*transform*/, uint32_t /*flags*/) = 0;
    
    virtual void visit(Scene *scene, const Mat4& parentTransform, uint32_t parentFlags);
    
    void setName(std::string& name) { _name = name; }
    const std::string& getName() { return _name; }
    
    virtual void setVisible(bool bVisible = true) { _bVisible = bVisible; }
    virtual bool isVisible() const { return _bVisible; }
    
    virtual Node* getParent() { return _parent; }
    virtual const Node* getParent() const { return _parent; }
    virtual void setParent(Node* parent) { _parent = parent; }

    virtual bool addChildren(Node * _child);
    virtual bool removeChildren(Node *_child);
    virtual void removeFromParent();
    virtual ssize_t getChildrenCount() const{ return _children.size(); }
    
    //Property
    virtual void setPosition(const Vec2 &position);
    virtual const Vec2& getPosition() const;
    virtual void setPositionZ(float positionZ);
    virtual float getPositionZ() const;
    virtual void setPosition3D(const Vec3& position);
    virtual Vec3 getPosition3D() const;

    virtual void setScaleX(float scaleX);
    virtual float getScaleX() const;

    virtual void setScaleY(float scaleY);
    virtual float getScaleY() const;
    
    virtual void setScaleZ(float scaleZ);
    virtual float getScaleZ() const;
    

    virtual void setAnchorPoint(const Vec2& anchorPoint);
    virtual const Vec2& getAnchorPoint() const;
    virtual const Vec2& getAnchorPointInPoints() const;

    virtual void setContentSize(const Size& contentSize);
    virtual const Size& getContentSize() const;

    virtual void setRotation(float rotation);
    virtual float getRotation() const;

    virtual void setRotation3D(const Vec3& rotation);
    virtual Vec3 getRotation3D() const;
    
    virtual void setRotationQuat(const Quaternion& quat);
    virtual Quaternion getRotationQuat() const;
    
    virtual void setIgnoreAnchorPointForPosition(bool ignore);
    virtual bool isIgnoreAnchorPointForPosition() const;

    virtual const Color4F& getColor() const { return _realColor; }
    virtual void setColor(const Color4F& color) { _realColor = color; }
    
    virtual const Mat4& getNodeToParentTransform() const;
    virtual Mat4 getNodeToParentTransform(Node* ancestor) const;
    
protected:
    // update quaternion from Rotation3D
    void updateRotationQuat();
    // update Rotation3D from quaternion
    void updateRotation3D();
    
    Mat4 transform(const Mat4 &parentTransform);
    
    std::string _name;
    void *_userData;
    bool _bVisible;
    
    bool _ignoreAnchorPointForPosition; ///< true if the Anchor Vec2 will be (0,0) when you position the Node, false otherwise.
    ///< Used by Layer and Scene.
    
    Mat4 _modelMatrix;
    
    Node *_parent;
    std::vector<Node *> _children;
    
    float _rotationX;               ///< rotation on the X-axis
    float _rotationY;               ///< rotation on the Y-axis
    
    // rotation Z is decomposed in 2 to simulate Skew for Flash animations
    float _rotationZ_X;             ///< rotation angle on Z-axis, component X
    float _rotationZ_Y;             ///< rotation angle on Z-axis, component Y
    
    Quaternion _rotationQuat;       ///rotation using quaternion, if _rotationZ_X == _rotationZ_Y, _rotationQuat = RotationZ_X * RotationY * RotationX, else _rotationQuat = RotationY * RotationX
    
    float _scaleX;                  ///< scaling factor on x-axis
    float _scaleY;                  ///< scaling factor on y-axis
    float _scaleZ;                  ///< scaling factor on z-axis
    
    Vec2 _position;                 ///< position of the node
    float _positionZ;               ///< OpenGL real Z position
    Vec2 _normalizedPosition;
    bool _usingNormalizedPosition;
    bool _normalizedPositionDirty;
    
    Vec2 _anchorPointInPoints;      ///< anchor point in points
    Vec2 _anchorPoint;              ///< anchor point normalized (NOT in points)
    
    Size _contentSize;              ///< untransformed size of the node
    bool _contentSizeDirty;         ///< whether or not the contentSize is dirty
    
    Mat4 _modelViewTransform;       ///< ModelView transform of the Node.
    
    Color4F     _realColor;
    // "cache" variables are allowed to be mutable
    mutable Mat4 _transform;        ///< transform
    mutable bool _transformDirty;   ///< transform dirty flag
    mutable Mat4 _inverse;          ///< inverse transform
    mutable bool _inverseDirty;     ///< inverse transform dirty flag
    mutable Mat4* _additionalTransform; ///< two transforms needed by additional transforms
    mutable bool _additionalTransformDirty; ///< transform dirty ?
    bool _transformUpdated;         ///< Whether or not the Transform object was updated since the last frame
};

GRAPHICCORE_END

#endif /* GRAPHICCORE_NODE_H */
