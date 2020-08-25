//
//  GcDuplicateNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_DUPLICATENODE_H
#define EFFECTEDITOR_DUPLICATENODE_H

#include "GcRenderNode.h"

GRAPHICCORE_BEGIN

/**
 * DuplicateNode which can duplicate an other RenderNode's render result
 */
class DuplicateNode : public RenderNode{
public:
    
    DuplicateNode(RenderNode* node);
    virtual ~DuplicateNode();
    
    virtual void draw(Scene* /*scene*/, const Mat4 & /*transform*/, uint32_t /*flags*/) override;
    
    const Color4F& getBKColor() const { return _bkColor; }
    void setBKColor(const Color4F& color) { _bkColor = color; }
private:
    RenderNode* _referenceNode;
    Color4F _bkColor;
};

CLASSREF(DuplicateNode)

GRAPHICCORE_END

#endif /* EFFECTEDITOR_DUPLICATENODE_H */
