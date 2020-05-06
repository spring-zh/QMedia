//
//  GcDuplicateNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcDuplicateNode.h"

GRAPHICCORE_BEGIN

DuplicateNode::DuplicateNode(RenderNode* node):_referenceNode(node){
    _renderRange = _referenceNode->getRange();
}

DuplicateNode::~DuplicateNode(){
}

void DuplicateNode::draw(Scene* scene, const Mat4 & transform, uint32_t flags)
{
    _referenceNode->duplicateDraw(scene, transform, this);
}

GRAPHICCORE_END
