//
//  GcNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "BaseNode.h"
#include "Utils/Comm.h"

namespace QMedia { namespace Api {

BaseNode::BaseNode() {
    setIgnoreAnchorPointForPosition(true);
}

void BaseNode::setPosition(const Point & position) {
    
    if (position_ == position)
        return;
    
    position_ = position;
    
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    usingNormalizedPosition_ = false;
}

void BaseNode::setPositionZ(int32_t positionz) {
    if (positionz_ == positionz)
        return;
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    positionz_ = positionz;
}

void BaseNode::setScaleX(float scalex) {
    if (scale_.v1 == scalex)
        return;
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    scale_.v1 = scalex;
}

void BaseNode::setScaleY(float scaley) {
   if (scale_.v2 == scaley)
        return;
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    scale_.v2 = scaley;
}
void BaseNode::setScaleZ(float scalez) {
    if (scale_.v3 == scalez)
        return;
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    scale_.v3 = scalez;
}

void BaseNode::setContentSize(const Size & content_size) {
    if (content_size_ != content_size) {
        content_size_ = content_size;
        anchorPointInPoints_.x = content_size.width * anchor_point_.x;
        anchorPointInPoints_.y = content_size.height * anchor_point_.y;
        transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    }
}

void BaseNode::setRotation(int32_t rotation) {
    if (rotationZ_X_ == rotation)
        return;
    rotationZ_X_ = rotationZ_Y_ = rotation;
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    updateRotationQuat();
}

void BaseNode::setRotation3d(const Vec3f & rotation3d) {
    if (rotation_.v1 == rotation3d.v1 &&
        rotation_.v2 == rotation3d.v2 &&
        rotationZ_X_ == rotation3d.v3)
        return;
    
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    
    rotation_.v1 = rotation3d.v1;
    rotation_.v2 = rotation3d.v2;
    
    // rotation Z is decomposed in 2 to simulate Skew for Flash animations
    rotationZ_Y_ = rotationZ_X_ = rotation3d.v3;
    
    updateRotationQuat();
}

void BaseNode::setAnchorPoint(const Point & anchor_point) {
    if (anchor_point_ == anchor_point)
        return;
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    anchor_point_ = anchor_point;
}

void BaseNode::setBlendFunc(const Blendfunc & bf) {
    if (blend_func_ == bf)
        return;
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    blend_func_ = bf;
}

void BaseNode::setCrop(const Rect4f & crop) {
    if (crop_ == crop)
        return;
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    crop_ = crop;
}

/// isRelativeAnchorPoint getter
bool BaseNode::isIgnoreAnchorPointForPosition() const
{
    return ignoreAnchorPointForPosition_;
}

/// isRelativeAnchorPoint setter
void BaseNode::setIgnoreAnchorPointForPosition(bool newValue)
{
    if (newValue != ignoreAnchorPointForPosition_)
    {
        ignoreAnchorPointForPosition_ = newValue;
        transformUpdated_ = transformDirty_ = inverseDirty_ = true;
    }
}

void BaseNode::updateRotationQuat() {
    // convert Euler angle to quaternion
      // when _rotationZ_X == _rotationZ_Y, _rotationQuat = RotationZ_X * RotationY * RotationX
      // when _rotationZ_X != _rotationZ_Y, _rotationQuat = RotationY * RotationX
    float halfRadx = DEGREES_TO_RADIANS(rotation_.v1 / 2.f), halfRady = DEGREES_TO_RADIANS(rotation_.v2 / 2.f), halfRadz = rotationZ_X_ == rotationZ_Y_ ? -DEGREES_TO_RADIANS(rotationZ_X_ / 2.f) : 0;
      float coshalfRadx = cosf(halfRadx), sinhalfRadx = sinf(halfRadx), coshalfRady = cosf(halfRady), sinhalfRady = sinf(halfRady), coshalfRadz = cosf(halfRadz), sinhalfRadz = sinf(halfRadz);
      rotationQuat_.x = sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz;
      rotationQuat_.y = coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz;
      rotationQuat_.z = coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz;
      rotationQuat_.w = coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz;
}

void BaseNode::updateRotation3D() {
    //convert quaternion to Euler angle
    float x = rotationQuat_.x, y = rotationQuat_.y, z = rotationQuat_.z, w = rotationQuat_.w;
    rotation_.v1 = atan2f(2.f * (w * x + y * z), 1.f - 2.f * (x * x + y * y));
    float sy = 2.f * (w * y - z * x);
    sy = RenderEngine::clampf(sy, -1, 1);
    rotation_.v2 = asinf(sy);
    rotationZ_X_ = atan2f(2.f * (w * z + x * y), 1.f - 2.f * (y * y + z * z));
    
    rotation_.v1 = RADIANS_TO_DEGREES(rotation_.v1);
    rotation_.v2 = RADIANS_TO_DEGREES(rotation_.v2);
    rotationZ_X_ = rotationZ_Y_ = -RADIANS_TO_DEGREES(rotationZ_X_);
}
void BaseNode::setRotationQuat(const Quaternion& quat) {
    rotationQuat_ = quat;
    updateRotation3D();
    transformUpdated_ = transformDirty_ = inverseDirty_ = true;
}
Quaternion BaseNode::getRotationQuat() const {
    return rotationQuat_;
}

const Mat4& BaseNode::getNodeToParentTransform() const {
    if (transformDirty_)
    {
        // Translate values
        float x = position_.x;
        float y = position_.y;
        float z = positionz_;
        
        if (ignoreAnchorPointForPosition_)
        {
            x += anchorPointInPoints_.x;
            y += anchorPointInPoints_.y;
        }
        
//        bool needsSkewMatrix = ( _skewX || _skewY );
        
        // Build Transform Matrix = translation * rotation * scale
        Mat4 translation;
        //move to anchor point first, then rotate
        Mat4::createTranslation(x, y, z, &translation);
        
        Mat4::createRotation(rotationQuat_, &transform_);
        
        if (rotationZ_X_ != rotationZ_Y_)
        {
            // Rotation values
            // Change rotation code to handle X and Y
            // If we skew with the exact same value for both x and y then we're simply just rotating
            float radiansX = -DEGREES_TO_RADIANS(rotationZ_X_);
            float radiansY = -DEGREES_TO_RADIANS(rotationZ_Y_);
            float cx = cosf(radiansX);
            float sx = sinf(radiansX);
            float cy = cosf(radiansY);
            float sy = sinf(radiansY);
            
            float m0 = transform_.m[0], m1 = transform_.m[1], m4 = transform_.m[4], m5 = transform_.m[5], m8 = transform_.m[8], m9 = transform_.m[9];
            transform_.m[0] = cy * m0 - sx * m1, transform_.m[4] = cy * m4 - sx * m5, transform_.m[8] = cy * m8 - sx * m9;
            transform_.m[1] = sy * m0 + cx * m1, transform_.m[5] = sy * m4 + cx * m5, transform_.m[9] = sy * m8 + cx * m9;
        }
        transform_ = translation * transform_;
        
        if (scale_.v1 != 1.f)
        {
            transform_.m[0] *= scale_.v1, transform_.m[1] *= scale_.v1, transform_.m[2] *= scale_.v1;
        }
        if (scale_.v2 != 1.f)
        {
            transform_.m[4] *= scale_.v2, transform_.m[5] *= scale_.v2, transform_.m[6] *= scale_.v2;
        }
        if (scale_.v3 != 1.f)
        {
            transform_.m[8] *= scale_.v3, transform_.m[9] *= scale_.v3, transform_.m[10] *= scale_.v3;
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
        if (!isZero(anchorPointInPoints_))
        {
            // FIXME:: Argh, Mat4 needs a "translate" method.
            // FIXME:: Although this is faster than multiplying a vec4 * mat4
            transform_.m[12] += transform_.m[0] * -anchorPointInPoints_.x + transform_.m[4] * -anchorPointInPoints_.y;
            transform_.m[13] += transform_.m[1] * -anchorPointInPoints_.x + transform_.m[5] * -anchorPointInPoints_.y;
            transform_.m[14] += transform_.m[2] * -anchorPointInPoints_.x + transform_.m[6] * -anchorPointInPoints_.y;
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
    
    transformDirty_ = additionalTransformDirty_ = false;
    
    return transform_;
}

Mat4 BaseNode::transform(const Mat4& parentTransform)
{
    return parentTransform * this->getNodeToParentTransform();
}

}
}
