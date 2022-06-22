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
#include "opengl/Texture2DDrawer.h"
#include <vector>

GRAPHICCORE_BEGIN

class Layer : public RenderNode{
public:
    Layer(Size size);
    virtual ~Layer();
    
    virtual void visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags) override;
    
    virtual void draw(Scene* scene, const Mat4 & transform, uint32_t flags) override ;
    
    //call by DuplicateNode
    virtual void duplicateDraw(Scene* /*scene*/, const Mat4 & /*transform*/, const Node* /*displayNode*/) override ;
    
    const Texture2D* getOutputTexture() override { return _texture.get(); }

    Size getLayerSize() const { return _layerSize; }
    virtual void setLayerSize(const Size& size) { _layerSize = size; }
    
    const Color4F& getBKColor() const { return _bkColor; }
    void setBKColor(const Color4F& color) { _bkColor = color; }
    
    void setEnable3d(bool enable3d) { _enable3d = enable3d; }
    bool getEnable3d() { return _enable3d; }
    
    const Scene* Scene() const {return &_scene;}
    
    virtual bool createRes() override;
    virtual void releaseRes() override;
    
protected:
    
    GraphicCore::Scene _scene;
    Color4F _bkColor;
    Size _layerSize;
    bool _enable3d;

    std::shared_ptr<FrameBuffer> _framebuffer;

    std::shared_ptr<Texture2D> _texture;
    std::unique_ptr<Texture2D> _depth_texture;
};

CLASSREF(Layer)

GRAPHICCORE_END

#endif /* GRAPHICCORE_LAYER_H */
