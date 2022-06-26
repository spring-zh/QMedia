//
//  Layer2.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_LAYER2_H
#define EFFECTEDITOR_LAYER2_H

#include "RenderEngine/SceneNode.h"
#include "RenderEngine/opengl/Texture2DDrawer.h"
#include "RenderEngine/opengl/TextureFrameBuffer.h"
#include <vector>

namespace QMedia { namespace Api {

using RenderEngine::Color4F;
using RenderEngine::TextureFrameBuffer;

class Layer2 : public SceneNode {
public:
    Layer2(RenderEngine::Size size);
    virtual ~Layer2();
    
    virtual void visit(Scene *scene, const Mat4& parentTransform, uint32_t parentFlags) override;
    
    virtual void draw(Scene* scene, const Mat4 & transform, uint32_t flags) override ;
    
    //call by DuplicateNode
    virtual void duplicateDraw(Scene* scene, const Mat4 & transform, SceneNode* displayNode) override ;

    RenderEngine::Size getLayerSize() const { return _layerSize; }
    virtual void setLayerSize(const RenderEngine::Size& size) { _layerSize = size; }
    
    const Api::Vec4f& getBKColor() const { return _bkColor; }
    void setBKColor(const Api::Vec4f& color) { _bkColor = color; }
    
    void setEnable3d(bool enable3d) { _enable3d = enable3d; }
    bool getEnable3d() { return _enable3d; }
    
    const RenderEngine::Scene* Scene() const {return &_scene;}
    
    virtual bool createRes() override;
    virtual void releaseRes() override;
    
protected:
    
    RenderEngine::Scene _scene;
    Api::Vec4f _bkColor{0,0,0,1};
    RenderEngine::Size _layerSize;
    bool _enable3d = false;

    std::unique_ptr<TextureFrameBuffer> fbo_texture_;
};

}
}

#endif /* EFFECTEDITOR_LAYER2_H */
