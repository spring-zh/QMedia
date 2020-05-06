//
//  GcLayer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_LAYER_H
#define GRAPHICCORE_LAYER_H

#include "GcRenderNode.h"
#include "GcFilter.h"
#include "opengl/GLEngine.h"
#include "opengl/ShaderProgram.h"
#include <vector>

GRAPHICCORE_BEGIN

class Layer : public RenderNode{
public:
    Layer(Size size);
    virtual ~Layer();
    
    virtual void draw(Scene* /*scene*/, const Mat4 & /*transform*/, uint32_t /*flags*/) override;
    
    virtual void visit(Scene *scene, const Mat4& parentTransform, uint32_t parentFlags) override;
    
    virtual Texture2D* getDuplicateTexture() const { return _texture2d ;}
    virtual void duplicateDraw(GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, const GraphicCore::Node* /*diaplayNode*/) override;
    
    const Color4F& getBKColor() const { return _bkColor; }
    void setBKColor(const Color4F& color) { _bkColor = color; }
    
    void addFilter(FilterRef filter);
    const Scene* Scene() const {return &_scene;}
    
    virtual bool createRes() override;
    virtual void releaseRes() override;
    
protected:
    
    GraphicCore::Scene _scene;
    Color4F _bkColor;
    Size _layerSize;
    
    Texture2D *_texture2d;
    FrameBuffer *_framebuffer;
    ShaderProgram _shaderProgram;
    
    std::vector<FilterRef> _fliters;
};

using LayerRef = std::shared_ptr<Layer>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_LAYER_H */
