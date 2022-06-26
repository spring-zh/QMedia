//
//  BaseNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QMEDIA_BASE_NODE_H
#define QMEDIA_BASE_NODE_H

#include "video_render_node.h"
#include "point.h"
#include "size.h"
#include "vec3f.h"
#include "vec4f.h"
#include "color4f.h"
#include "rect4f.h"
#include "animator.h"
#include "blendfunc.h"
#include "GraphicCore/opengl/GLMacros.h"
#include "GraphicCore/math/CCGeometry.h"

namespace QMedia { namespace Api {

inline bool isZero(Point p) {
    return p.x == 0.0f && p.y == 0.0f;
}

using GraphicCore::Mat4;
using GraphicCore::Quaternion;

class BaseNode : public VideoRenderNode {
public:
    BaseNode() ;
    virtual ~BaseNode() {}
    
    virtual void setVisible(bool visible) override { visible_ = visible; }

    virtual bool getVisible() override { return visible_;}

    virtual void setPosition(const Point & position) override ;

    virtual Point getPosition() override { return position_;}

    virtual void setPositionZ(int32_t positionz) override ;

    virtual int32_t getPositionZ() override { return positionz_; }

    virtual void setScaleX(float scalex) override ;

    virtual float getScaleX() override { return scale_.v1; }

    virtual void setScaleY(float scaley) override ;

    virtual float getScaleY() override { return scale_.v2; }

    virtual void setScaleZ(float scalez) override ;

    virtual float getScaleZ() override { return scale_.v3; }

    virtual void setContentSize(const Size & content_size) override ;

    virtual Size getContentSize() override { return content_size_;}

    virtual void setRotation(int32_t rotation) override ;

    virtual int32_t getRotation() override { return rotationZ_X_; }

    virtual void setRotation3d(const Vec3f & rotation3d) override ;

    virtual Vec3f getRotation3d() override { return Vec3f(rotation_.v1,rotation_.v2,rotationZ_X_); }

    virtual void setAnchorPoint(const Point & anchor_point) override ;

    virtual Point getAnchorPoint() override { return anchor_point_; }

    virtual void setColor4F(const Color4f & color) override { color_ = color; }

    virtual Color4f getColor4F() override { return color_;}
    
    virtual void setAlpha(float alpha) override { color_.a = alpha; }

    virtual float getAlpha() { return color_.a; }

    virtual void setBlendFunc(const Blendfunc & bf) override ;

    virtual Blendfunc getBlendFunc() override { return blend_func_; }

    virtual void setCrop(const Rect4f & crop) override ;

    virtual Rect4f getCrop() override { return crop_; }
    
    // calculate
    const Mat4& getNodeToParentTransform() const;
    
    Mat4 transform(const Mat4& parentTransform);

protected:
    virtual void setIgnoreAnchorPointForPosition(bool ignore);
    virtual bool isIgnoreAnchorPointForPosition() const;
    virtual void setRotationQuat(const Quaternion& quat);
    virtual Quaternion getRotationQuat() const;
    void updateRotationQuat();
    void updateRotation3D();
    
    bool visible_{true};
    Point position_{0,0};
    int32_t positionz_{0};
    Vec3f scale_{1,1,1};
    Size content_size_{0,0};
    Vec3f rotation_{0,0,0};
    Point anchor_point_{0,0};
    Color4f color_{1,1,1,1};
    Blendfunc blend_func_{Blendfunc::ONE, Blendfunc::ZERO};
    Rect4f crop_{0,0,0,0};
    
    //
    GraphicCore::Mat4 model_matrix_;
    GraphicCore::Vec2 normalizedPosition_;
    bool usingNormalizedPosition_;
    bool normalizedPositionDirty_;
    
    float rotationZ_X_{0};             ///< rotation angle on Z-axis, component X
    float rotationZ_Y_{0};             ///< rotation angle on Z-axis, component Y
    
    bool ignoreAnchorPointForPosition_; ///< true if the Anchor Vec2 will be (0,0) when you position the Node, false
    Point anchorPointInPoints_{0,0};      ///< anchor point in points
    
    GraphicCore::Mat4 modelViewTransform_;
    
    GraphicCore::Quaternion rotationQuat_;       ///rotation using quaternion, if _rotationZ_X == _rotationZ_Y, _rotationQuat = RotationZ_X * RotationY * RotationX, else _rotationQuat = RotationY * RotationX
    
    // "cache" variables are allowed to be mutable
    mutable GraphicCore::Mat4 transform_;        ///< transform
    mutable bool transformDirty_;   ///< transform dirty flag
    mutable GraphicCore::Mat4 inverse_;          ///< inverse transform
    mutable bool inverseDirty_;     ///< inverse transform dirty flag
    mutable GraphicCore::Mat4* additionalTransform_ = NULL; ///< two transforms needed by additional transforms
    mutable bool additionalTransformDirty_; ///< transform dirty ?
    bool transformUpdated_;         ///< Whether or not the Transform object was updated since the last frame
};

}
}

#endif /* QMEDIA_BASE_NODE_H */
