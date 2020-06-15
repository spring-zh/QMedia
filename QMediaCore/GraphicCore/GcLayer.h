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
#include "texture_filter/FilterFactory.h"
#include "opengl/GLEngine.h"
#include "opengl/ShaderProgram.h"
#include "opengl/Texture2DDrawer.h"
#include <vector>

GRAPHICCORE_BEGIN

class Layer : public RenderNode{
public:
    Layer(Size size);
    virtual ~Layer();
    
    virtual void visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags) override;
    
    virtual Texture2D* getDuplicateTexture() const { return _texture_first ;}
    
    virtual void draw(Scene* scene, const Mat4 & transform, uint32_t flags) override ;
    
    //call by DuplicateNode
    virtual void duplicateDraw(Scene* /*scene*/, const Mat4 & /*transform*/, const Node* /*displayNode*/) override ;

    Size getLayerSize() const { return _layerSize; }
    
    const Color4F& getBKColor() const { return _bkColor; }
    void setBKColor(const Color4F& color) { _bkColor = color; }
    
    const Scene* Scene() const {return &_scene;}
    
    virtual bool createRes() override;
    virtual void releaseRes() override;
    
    class FilterObject {
    public:
        FilterObject():_filter(nullptr), _range(0,0) {}
        FilterObject(TextureFilter* filter, Range<int64_t> range):_filter(filter), _range(range) {}
        TextureFilter* _filter;
        Range<int64_t> _range;
    };
    
    using FilterObjectRef = std::shared_ptr<FilterObject>;
    
    void addFilter(FilterObjectRef filterObjectRef);
protected:
    
    GraphicCore::Scene _scene;
    Color4F _bkColor;
    Size _layerSize;

    FrameBuffer *_framebuffer;

    Texture2D *_texture_first, *_texture_second;
    std::shared_ptr<Texture2DDrawer> _textureDrawer;

    std::vector<FilterObjectRef> _fliter_group;
};

using LayerRef = std::shared_ptr<Layer>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_LAYER_H */
