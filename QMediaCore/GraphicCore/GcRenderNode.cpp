//
//  GcRenderNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcRenderNode.h"

GRAPHICCORE_BEGIN

RenderNode::RenderNode(){
    
}

RenderNode::~RenderNode(){
    
}

void RenderNode::visit(Scene *scene, const Mat4& parentTransform, uint32_t parentFlags)
{
    Range<int64_t> dispRange = getRange();
    if (scene->getDelta() >= dispRange._start && scene->getDelta() <= dispRange._end) {
        Node::visit(scene, parentTransform, parentFlags);
    }
}

bool RenderNode::createRes() {
    bool bRet = true;
    for (auto& child : _children) {
        RenderNode* renderNode = dynamic_cast<RenderNode*>(child);
        if (renderNode != nullptr) {
            bRet &= renderNode->createRes();
        }
    }
    return bRet;
}
void RenderNode::releaseRes() {
    for (auto& child : _children) {
        RenderNode* renderNode = dynamic_cast<RenderNode*>(child);
        if (renderNode != nullptr) {
            renderNode->releaseRes();
        }
    }
}

GRAPHICCORE_END
