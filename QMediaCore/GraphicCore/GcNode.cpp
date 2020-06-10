//
//  GcNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcNode.h"

GRAPHICCORE_BEGIN

Node::Node():
_rotationX(0),
_rotationY(0),
_rotationZ_X(0),
_rotationZ_Y(0),
_positionZ(0),
_scaleX(1.0f),
_scaleY(1.0f),
_scaleZ(1.0f),
_realColor(1, 1, 1, 1),
_bVisible(true),
_parent(nullptr),
_additionalTransform(nullptr)
{
    setIgnoreAnchorPointForPosition(true);
}

void Node::visit(Scene *scene, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (!_bVisible)
    {
        return;
    }
    uint32_t selfFlags = Flags_None;
    if (_transformDirty || _contentSizeDirty || (parentFlags & Flags_Update)) {
        _modelViewTransform = this->transform(parentTransform);
        selfFlags = Flags_Update;
    }
    _transformUpdated = false;
    _contentSizeDirty = false;
    
    scene->pushMatrix(MATRIX_STACK_MODELVIEW);
    scene->loadMatrix(MATRIX_STACK_MODELVIEW, _modelViewTransform);
    
    this->draw(scene, _modelViewTransform, selfFlags);
    if(!_children.empty()){
        for (auto& node : _children) {
            node->visit(scene, _modelViewTransform, selfFlags);
        }
    }
    scene->popMatrix(MATRIX_STACK_MODELVIEW);
}

bool Node::addChildren(Node * _child) {
    if (_child->getParent()) {
        return false;
    }
    auto iter = std::find(_children.begin(), _children.end(), _child);
    if(iter != _children.end()){
        return false;
    }
    _child->setParent(this);
    _children.push_back(_child);
    return true;
}
bool Node::removeChildren(Node *_child) {
    auto iter = std::find(_children.begin(), _children.end(), _child);
    if(iter == _children.end()){
        return false;
    }
    _children.erase(iter);
    return true;
}
void Node::removeFromParent() {
    if (getParent()) {
        getParent()->removeChildren(this);
    }
}

const Vec2& Node::getAnchorPointInPoints() const
{
    return _anchorPointInPoints;
}

/// anchorPoint getter
const Vec2& Node::getAnchorPoint() const
{
    return _anchorPoint;
}

void Node::setAnchorPoint(const Vec2& point)
{
    if (! point.equals(_anchorPoint))
    {
        _anchorPoint = point;
        _anchorPointInPoints.set(_contentSize.width * _anchorPoint.x, _contentSize.height * _anchorPoint.y);
        _transformUpdated = _transformDirty = _inverseDirty = true;
    }
}

/// isRelativeAnchorPoint getter
bool Node::isIgnoreAnchorPointForPosition() const
{
    return _ignoreAnchorPointForPosition;
}

/// isRelativeAnchorPoint setter
void Node::setIgnoreAnchorPointForPosition(bool newValue)
{
    if (newValue != _ignoreAnchorPointForPosition)
    {
        _ignoreAnchorPointForPosition = newValue;
        _transformUpdated = _transformDirty = _inverseDirty = true;
    }
}

/// contentSize getter
const Size& Node::getContentSize() const
{
    return _contentSize;
}

void Node::setContentSize(const Size & size)
{
    if (! size.equals(_contentSize))
    {
        _contentSize = size;
        
        _anchorPointInPoints.set(_contentSize.width * _anchorPoint.x, _contentSize.height * _anchorPoint.y);
        _transformUpdated = _transformDirty = _inverseDirty = _contentSizeDirty = true;
    }
}

/// scaleX getter
float Node::getScaleX() const
{
    return _scaleX;
}

/// scaleX setter
void Node::setScaleX(float scaleX)
{
    if (_scaleX == scaleX)
        return;
    
    _scaleX = scaleX;
    _transformUpdated = _transformDirty = _inverseDirty = true;
}

/// scaleY getter
float Node::getScaleY() const
{
    return _scaleY;
}

/// scaleY setter
void Node::setScaleZ(float scaleZ)
{
    if (_scaleZ == scaleZ)
        return;
    
    _scaleZ = scaleZ;
    _transformUpdated = _transformDirty = _inverseDirty = true;
}

/// scaleY getter
float Node::getScaleZ() const
{
    return _scaleZ;
}

/// scaleY setter
void Node::setScaleY(float scaleY)
{
    if (_scaleY == scaleY)
        return;
    
    _scaleY = scaleY;
    _transformUpdated = _transformDirty = _inverseDirty = true;
}

void Node::setRotation(float rotation)
{
    if (_rotationZ_X == rotation)
        return;
    
    _rotationZ_X = _rotationZ_Y = rotation;
    _transformUpdated = _transformDirty = _inverseDirty = true;
    
    updateRotationQuat();
}
float Node::getRotation() const
{
    return _rotationZ_X;
}

void Node::setRotation3D(const Vec3& rotation)
{
    if (_rotationX == rotation.x &&
        _rotationY == rotation.y &&
        _rotationZ_X == rotation.z)
        return;
    
    _transformUpdated = _transformDirty = _inverseDirty = true;
    
    _rotationX = rotation.x;
    _rotationY = rotation.y;
    
    // rotation Z is decomposed in 2 to simulate Skew for Flash animations
    _rotationZ_Y = _rotationZ_X = rotation.z;
    
    updateRotationQuat();
}

Vec3 Node::getRotation3D() const
{
    // rotation Z is decomposed in 2 to simulate Skew for Flash animations
    CCASSERT(_rotationZ_X == _rotationZ_Y, "_rotationZ_X != _rotationZ_Y");
    
    return Vec3(_rotationX,_rotationY,_rotationZ_X);
}

void Node::setRotationQuat(const Quaternion& quat)
{
    _rotationQuat = quat;
    updateRotation3D();
    _transformUpdated = _transformDirty = _inverseDirty = true;
}

Quaternion Node::getRotationQuat() const
{
    return _rotationQuat;
}

/// position getter
const Vec2& Node::getPosition() const
{
    return _position;
}

/// position setter
void Node::setPosition(const Vec2& position)
{
//    setPosition(position.x, position.y);
    if (_position.x == position.x && _position.y == position.y)
        return;
    
    _position.x = position.x;
    _position.y = position.y;
    
    _transformUpdated = _transformDirty = _inverseDirty = true;
    _usingNormalizedPosition = false;
}
float Node::getPositionZ() const
{
    return _positionZ;
}

void Node::setPositionZ(float positionZ)
{
    if (_positionZ == positionZ)
        return;
    
    _transformUpdated = _transformDirty = _inverseDirty = true;
    
    _positionZ = positionZ;
}
void Node::setPosition3D(const Vec3& position)
{
    setPositionZ(position.z);
    setPosition(Vec2(position.x,position.y));
}

Vec3 Node::getPosition3D() const
{
    return Vec3(_position.x, _position.y, _positionZ);
}

const Mat4& Node::getNodeToParentTransform() const
{
    if (_transformDirty)
    {
        // Translate values
        float x = _position.x;
        float y = _position.y;
        float z = _positionZ;
        
        if (_ignoreAnchorPointForPosition)
        {
            x += _anchorPointInPoints.x;
            y += _anchorPointInPoints.y;
        }
        
//        bool needsSkewMatrix = ( _skewX || _skewY );
        
        // Build Transform Matrix = translation * rotation * scale
        Mat4 translation;
        //move to anchor point first, then rotate
        Mat4::createTranslation(x, y, z, &translation);
        
        Mat4::createRotation(_rotationQuat, &_transform);
        
        if (_rotationZ_X != _rotationZ_Y)
        {
            // Rotation values
            // Change rotation code to handle X and Y
            // If we skew with the exact same value for both x and y then we're simply just rotating
            float radiansX = -DEGREES_TO_RADIANS(_rotationZ_X);
            float radiansY = -DEGREES_TO_RADIANS(_rotationZ_Y);
            float cx = cosf(radiansX);
            float sx = sinf(radiansX);
            float cy = cosf(radiansY);
            float sy = sinf(radiansY);
            
            float m0 = _transform.m[0], m1 = _transform.m[1], m4 = _transform.m[4], m5 = _transform.m[5], m8 = _transform.m[8], m9 = _transform.m[9];
            _transform.m[0] = cy * m0 - sx * m1, _transform.m[4] = cy * m4 - sx * m5, _transform.m[8] = cy * m8 - sx * m9;
            _transform.m[1] = sy * m0 + cx * m1, _transform.m[5] = sy * m4 + cx * m5, _transform.m[9] = sy * m8 + cx * m9;
        }
        _transform = translation * _transform;
        
        if (_scaleX != 1.f)
        {
            _transform.m[0] *= _scaleX, _transform.m[1] *= _scaleX, _transform.m[2] *= _scaleX;
        }
        if (_scaleY != 1.f)
        {
            _transform.m[4] *= _scaleY, _transform.m[5] *= _scaleY, _transform.m[6] *= _scaleY;
        }
        if (_scaleZ != 1.f)
        {
            _transform.m[8] *= _scaleZ, _transform.m[9] *= _scaleZ, _transform.m[10] *= _scaleZ;
        }
        
        // FIXME:: Try to inline skew
        // If skew is needed, apply skew and then anchor point
//        if (needsSkewMatrix)
//        {
//            float skewMatArray[16] =
//            {
//                1, (float)tanf(DEGREES_TO_RADIANS(_skewY)), 0, 0,
//                (float)tanf(DEGREES_TO_RADIANS(_skewX)), 1, 0, 0,
//                0,  0,  1, 0,
//                0,  0,  0, 1
//            };
//            Mat4 skewMatrix(skewMatArray);
//
//            _transform = _transform * skewMatrix;
//        }
        
        // adjust anchor point
        if (!_anchorPointInPoints.isZero())
        {
            // FIXME:: Argh, Mat4 needs a "translate" method.
            // FIXME:: Although this is faster than multiplying a vec4 * mat4
            _transform.m[12] += _transform.m[0] * -_anchorPointInPoints.x + _transform.m[4] * -_anchorPointInPoints.y;
            _transform.m[13] += _transform.m[1] * -_anchorPointInPoints.x + _transform.m[5] * -_anchorPointInPoints.y;
            _transform.m[14] += _transform.m[2] * -_anchorPointInPoints.x + _transform.m[6] * -_anchorPointInPoints.y;
        }
    }
    
    //TODO: use _additionalTransform to adjust y-coordinate
//    if (_additionalTransform)
//    {
//        // This is needed to support both Node::setNodeToParentTransform() and Node::setAdditionalTransform()
//        // at the same time. The scenario is this:
//        // at some point setNodeToParentTransform() is called.
//        // and later setAdditionalTransform() is called every time. And since _transform
//        // is being overwritten everyframe, _additionalTransform[1] is used to have a copy
//        // of the last "_transform without _additionalTransform"
//        if (_transformDirty)
//            _additionalTransform[1] = _transform;
//        
//        if (_transformUpdated)
//            _transform = _additionalTransform[1] * _additionalTransform[0];
//    }
    
    _transformDirty = _additionalTransformDirty = false;
    
    return _transform;
}

Mat4 Node::getNodeToParentTransform(Node* ancestor) const
{
    Mat4 t(this->getNodeToParentTransform());
    
    for (Node *p = _parent;  p != nullptr && p != ancestor ; p = p->getParent())
    {
        t = p->getNodeToParentTransform() * t;
    }
    
    return t;
}

Mat4 Node::transform(const Mat4& parentTransform)
{
    return parentTransform * this->getNodeToParentTransform();
}

void Node::updateRotationQuat()
{
    // convert Euler angle to quaternion
    // when _rotationZ_X == _rotationZ_Y, _rotationQuat = RotationZ_X * RotationY * RotationX
    // when _rotationZ_X != _rotationZ_Y, _rotationQuat = RotationY * RotationX
    float halfRadx = DEGREES_TO_RADIANS(_rotationX / 2.f), halfRady = DEGREES_TO_RADIANS(_rotationY / 2.f), halfRadz = _rotationZ_X == _rotationZ_Y ? -DEGREES_TO_RADIANS(_rotationZ_X / 2.f) : 0;
    float coshalfRadx = cosf(halfRadx), sinhalfRadx = sinf(halfRadx), coshalfRady = cosf(halfRady), sinhalfRady = sinf(halfRady), coshalfRadz = cosf(halfRadz), sinhalfRadz = sinf(halfRadz);
    _rotationQuat.x = sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz;
    _rotationQuat.y = coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz;
    _rotationQuat.z = coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz;
    _rotationQuat.w = coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz;
}

void Node::updateRotation3D()
{
    //convert quaternion to Euler angle
    float x = _rotationQuat.x, y = _rotationQuat.y, z = _rotationQuat.z, w = _rotationQuat.w;
    _rotationX = atan2f(2.f * (w * x + y * z), 1.f - 2.f * (x * x + y * y));
    float sy = 2.f * (w * y - z * x);
    sy = clampf(sy, -1, 1);
    _rotationY = asinf(sy);
    _rotationZ_X = atan2f(2.f * (w * z + x * y), 1.f - 2.f * (y * y + z * z));
    
    _rotationX = RADIANS_TO_DEGREES(_rotationX);
    _rotationY = RADIANS_TO_DEGREES(_rotationY);
    _rotationZ_X = _rotationZ_Y = -RADIANS_TO_DEGREES(_rotationZ_X);
}

GRAPHICCORE_END
