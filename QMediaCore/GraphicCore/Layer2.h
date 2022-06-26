//
//  Layer2.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_LAYER2_H
#define EFFECTEDITOR_LAYER2_H

#include "GraphicCore/SceneNode.h"
#include "GraphicCore/opengl/Texture2DDrawer.h"
#include "GraphicCore/opengl/TextureFrameBuffer.h"
#include <vector>

namespace QMedia { namespace Api {

using GraphicCore::Color4F;
using GraphicCore::TextureFrameBuffer;

class Layer2 : public SceneNode {
public:
    Layer2(GraphicCore::Size size);
    virtual ~Layer2();
    
    virtual void visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags) override;
    
    virtual void draw(GraphicCore::Scene* scene, const Mat4 & transform, uint32_t flags) override ;
    
    //call by DuplicateNode
    virtual void duplicateDraw(GraphicCore::Scene* scene, const Mat4 & transform, SceneNode* displayNode) override ;

    GraphicCore::Size getLayerSize() const { return _layerSize; }
    virtual void setLayerSize(const GraphicCore::Size& size) { _layerSize = size; }
    
    const GraphicCore::Color4F& getBKColor() const { return _bkColor; }
    void setBKColor(const GraphicCore::Color4F& color) { _bkColor = color; }
    
    void setEnable3d(bool enable3d) { _enable3d = enable3d; }
    bool getEnable3d() { return _enable3d; }
    
    const GraphicCore::Scene* Scene() const {return &_scene;}
    
    virtual bool createRes() override;
    virtual void releaseRes() override;
    
protected:
    
    GraphicCore::Scene _scene;
    GraphicCore::Color4F _bkColor{0,0,0,1};
    GraphicCore::Size _layerSize;
    bool _enable3d = false;

    std::unique_ptr<TextureFrameBuffer> fbo_texture_;
};

}
}

#endif /* EFFECTEDITOR_LAYER2_H */
