//
//  SceneNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "SceneNode.h"

namespace QMedia { namespace Api {

SceneNode::SceneNode():_flipMode(GraphicCore::Drawable2D::NONE){
    
}

SceneNode::~SceneNode(){
    
}

bool SceneNode::addChildren(SceneNode * _child) {
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
bool SceneNode::removeChildren(SceneNode *_child) {
    auto iter = std::find(_children.begin(), _children.end(), _child);
    if(iter == _children.end()){
        return false;
    }
    _children.erase(iter);
    return true;
}
void SceneNode::removeFromParent() {
    if (getParent()) {
        ((SceneNode*)getParent())->removeChildren(this);
        setParent(nullptr);
    }
}

void SceneNode::visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags)
{
    Range<int64_t> dispRange = getRange();
    if ( dispRange.isValid() && !(scene->getDelta() >= dispRange._start && scene->getDelta() <= dispRange._end))
        return;

    AnimateNode::updateNodeByAnimations(scene->getDelta() - dispRange._start);

    __visit(scene, parentTransform, parentFlags);
}

bool SceneNode::drawEffects(int64_t duration, const GraphicCore::Scene *scene, const Mat4 & transform, const GraphicCore::Texture2D *inputTexture) {
    const GraphicCore::Texture2D *output = _effect_group.drawEffects(duration, scene->getGLEngine(), inputTexture);
    if (output) {
//        _textureDrawer->draw(output, scene, transform, this, _flipMode);
        return true;
    }
    return false;
}

void SceneNode::addEffect(GraphicCore::EffectRef effectRef) {
    _effect_group.addEffect(effectRef);
}
void SceneNode::removeEffect(GraphicCore::EffectRef effectRef) {
    _effect_group.removeEffect(effectRef);
}

bool SceneNode::createRes() {
    bool bRet = true;
    _textureDrawer = std::shared_ptr<GraphicCore::Texture2DDrawer>(new GraphicCore::Texture2DDrawer());
    for (auto& child : _children) {
        bRet &= child->createRes();
    }
    return bRet;
}
void SceneNode::releaseRes() {
    _effect_group.release();
    _textureDrawer.reset();
    
    for (auto& child : _children) {
        child->releaseRes();
    }
}

void SceneNode::__visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags) {
    if (!visible_)
    {
        return;
    }
    uint32_t selfFlags = Flags_None;
    if (transformDirty_ || (parentFlags & Flags_Update)) {
        modelViewTransform_ = this->transform(parentTransform);
        selfFlags = Flags_Update;
    }
    transformUpdated_ = false;
    
    scene->pushMatrix(GraphicCore::MATRIX_STACK_MODELVIEW);
    scene->loadMatrix(GraphicCore::MATRIX_STACK_MODELVIEW, modelViewTransform_);
    
    this->draw(scene, modelViewTransform_, selfFlags);
    if(!_children.empty()){
        for (auto& node : _children) {
            node->visit(scene, modelViewTransform_, selfFlags);
        }
    }
    scene->popMatrix(GraphicCore::MATRIX_STACK_MODELVIEW);
}

Mat4 SceneNode::getNodeToParentTransform(SceneNode* ancestor) const {
    Mat4 t(BaseNode::getNodeToParentTransform());
    
    for (SceneNode *p = getParent();  p != nullptr && p != ancestor ; p = p->getParent())
    {
        t = p->BaseNode::getNodeToParentTransform() * t;
    }
    
    return t;
}

}
}
