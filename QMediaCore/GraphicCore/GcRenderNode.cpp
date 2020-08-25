//
//  GcRenderNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcRenderNode.h"

GRAPHICCORE_BEGIN

RenderNode::RenderNode():_flipMode(Drawable2D::NONE){
    
}

RenderNode::~RenderNode(){
    
}

void RenderNode::visit(Scene *scene, const Mat4& parentTransform, uint32_t parentFlags)
{
    Range<int64_t> dispRange = getRange();
    if ( dispRange.isValid() && !(scene->getDelta() >= dispRange._start && scene->getDelta() <= dispRange._end))
        return;

    AnimaNode::updateAnimations(scene->getDelta() - dispRange._start);

    Node::visit(scene, parentTransform, parentFlags);
}

bool RenderNode::drawEffects(int64_t duration, const Scene *scene, const Mat4 & transform, const Texture2D *inputTexture) {
    const Texture2D *output = _effect_group.drawEffects(duration, scene->getGLEngine(), inputTexture);
    if (output) {
        _textureDrawer->draw(output, scene, transform, this, _flipMode);
        return true;
    }
    return false;
}

void RenderNode::addEffect(EffectRef effectRef) {
    _effect_group.addEffect(effectRef);
}
void RenderNode::removeEffect(EffectRef effectRef) {
    _effect_group.removeEffect(effectRef);
}

bool RenderNode::createRes() {
    bool bRet = true;
    _textureDrawer = std::shared_ptr<Texture2DDrawer>(new Texture2DDrawer());
    for (auto& child : _children) {
        RenderNode* renderNode = dynamic_cast<RenderNode*>(child);
        if (renderNode != nullptr) {
            bRet &= renderNode->createRes();
        }
    }
    return bRet;
}
void RenderNode::releaseRes() {
    _effect_group.release();
    _textureDrawer.reset();
    
    for (auto& child : _children) {
        RenderNode* renderNode = dynamic_cast<RenderNode*>(child);
        if (renderNode != nullptr) {
            renderNode->releaseRes();
        }
    }
}

GRAPHICCORE_END
