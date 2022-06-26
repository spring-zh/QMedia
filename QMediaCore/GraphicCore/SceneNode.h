//
//  SceneNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QMEDIA_SCENE_NODE_H
#define QMEDIA_SCENE_NODE_H

#include "AnimateNode.h"
#include "effect/EffectGroup.h"
#include "opengl/Texture2DDrawer.h"

namespace QMedia { namespace Api {

enum ParentFlags : int {
    Flags_None = 0,
    Flags_Update = 1
};


class SceneNode : public AnimateNode {
public:
    
    SceneNode();
    virtual ~SceneNode();
    
    SceneNode* getParent() const { return parent_; }
    void setParent(SceneNode* parent) { parent_ = parent; }
    
    virtual bool addChildren(SceneNode * _child);
    virtual bool removeChildren(SceneNode *_child);
    virtual void removeFromParent();
    virtual ssize_t getChildrenCount() const{ return _children.size(); }
    
    virtual Mat4 getNodeToParentTransform(SceneNode* ancestor) const;
    
    virtual void draw(GraphicCore::Scene* scene, const Mat4 & transform, uint32_t flags) {
        return duplicateDraw(scene, transform, this);
    }
    virtual void visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags) ;
    
    //call by DuplicateNode
    virtual void duplicateDraw(GraphicCore::Scene* scene, const Mat4 & transform, SceneNode* displayNode) {}
    virtual const GraphicCore::Texture2D* getOutputTexture() { return nullptr; }
    
    virtual const Range<int64_t> getRange() { return _renderRange; }
    virtual void setRange(Range<int64_t> range) { _renderRange = range; }
    
    void addEffect(GraphicCore::EffectRef effectRef);
    void removeEffect(GraphicCore::EffectRef effectRef);
    
    void setFlipMode(GraphicCore::Drawable2D::FlipMode flipMode) { _flipMode = flipMode; }
    GraphicCore::Drawable2D::FlipMode getFlipMode() { return _flipMode; }
    
    // create/release gl resource
    virtual bool createRes();
    virtual void releaseRes();

protected:
    void __visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags);
    SceneNode* parent_{nullptr};
    std::vector<SceneNode *> _children;
    Range<int64_t> _renderRange;
    
    GraphicCore::Drawable2D::FlipMode _flipMode;
    
    //effects
    bool drawEffects(int64_t duration, const GraphicCore::Scene *scene, const Mat4 & transform, const GraphicCore::Texture2D *inputTexture);
    std::shared_ptr<GraphicCore::Texture2DDrawer> _textureDrawer;
    GraphicCore::EffectGroup _effect_group;
};

}
}

#endif /* QMEDIA_SCENE_NODE_H */
