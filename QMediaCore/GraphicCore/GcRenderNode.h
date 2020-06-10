//
//  GcRenderNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_RENDERNODE_H
#define GRAPHICCORE_RENDERNODE_H

#include "GcAnimaNode.h"

GRAPHICCORE_BEGIN

class RenderNode : public AnimaNode{
public:
    
    RenderNode();
    virtual ~RenderNode();
    
    virtual void draw(Scene* scene, const Mat4 & transform, uint32_t flags) override {
        return duplicateDraw(scene, transform, this);
    }
    virtual void visit(Scene *scene, const Mat4& parentTransform, uint32_t parentFlags) override;
    
    //call by DuplicateNode
    virtual void duplicateDraw(Scene* /*scene*/, const Mat4 & /*transform*/, const Node* /*displayNode*/) {}
    
    virtual const Range<int64_t> getRange() { return _renderRange; }
    virtual void setRange(Range<int64_t> range) { _renderRange = range; }
    
    // create/release gl resource
    virtual bool createRes();
    virtual void releaseRes();

protected:
    Range<int64_t> _renderRange;
    
};

using RenderNodeRef = std::shared_ptr<RenderNode>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_RENDERNODE_H */
