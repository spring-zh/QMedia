//
//  GcLayer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Layer2.h"
#include "Utils/Comm.h"
#include "RenderEngine/opengl/Drawable2D.h"
#include "RenderEngine/opengl/shader_code.h"

//extern "C" void DumpRGBAPixelToUIImage(unsigned char * buffer, int width, int height );

namespace QMedia { namespace Api {

Layer2::Layer2(RenderEngine::Size size): _layerSize(size) {
}

Layer2::~Layer2(){
    
}

void Layer2::visit(RenderEngine::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags)
{
    Range<int64_t> dispRange = getRange();
    if ( dispRange.isValid() && (scene->getDelta() < dispRange._start || scene->getDelta() > dispRange._end))
        return; //isn't in display time range

    AnimateNode::updateNodeByAnimations(scene->getDelta() - getRange()._start);
    
    if (!visible_)
    {
        return;
    }
    
    if (transformDirty_ || (parentFlags & Flags_Update) ) {
        modelViewTransform_ = BaseNode::transform(parentTransform);
    }
    transformUpdated_ = false;

    scene->pushMatrix(MATRIX_STACK_MODELVIEW);
    scene->loadMatrix(MATRIX_STACK_MODELVIEW, modelViewTransform_);

    RenderEngine::RenderDeviceGL *gle = scene->getGLEngine();
    gle->saveStatus();
    bool isDepthTest = gle->enableDepthTest();
    gle->setEnableDepthTest(_enable3d);
    // draw layer
    if ((int)_layerSize.width != fbo_texture_->width() || (int)_layerSize.height != fbo_texture_->height()) {
        //TODO: reset layer size
        fbo_texture_.reset();
        fbo_texture_ = std::unique_ptr<TextureFrameBuffer>(TextureFrameBuffer::createNew(_layerSize.width, _layerSize.height, _enable3d, true));
        RenderEngine::Rect svp = RenderEngine::Rect(0,0,_layerSize.width,_layerSize.height);
        _scene.setViewPort(svp);
        _scene.setProjection(PROJ_3D);
    }
//    gle->setCurrentFrameBuffer(_framebuffer.get());
    fbo_texture_->use();
    Api::Vec4f layerViewPort(0, 0, fbo_texture_->width(),fbo_texture_->height());
    gle->setViewPort(layerViewPort);
    gle->setClearColor(_bkColor);
    gle->clearByColor();
    
    if(!_children.empty()){
        //update timestamp
        _scene.setDelta(scene->getDelta());
        _scene.setGLEngine(scene->getGLEngine());
        //draw children with self's scene
        Mat4 layerTransform;
        for (auto& node : _children) {
            node->visit(&_scene, layerTransform, parentFlags);
        }
    }
    
//    std::vector<uint8_t> dddd(fbo_texture_->width()*fbo_texture_->height()*4);
//    glReadPixels(0, 0, fbo_texture_->width(), fbo_texture_->height(), GL_RGBA, GL_UNSIGNED_BYTE, dddd.data());
//    DumpRGBAPixelToUIImage(dddd.data(), fbo_texture_->width(), fbo_texture_->height());
    
    gle->recoverStatus();
    gle->setEnableDepthTest(isDepthTest);
    this->draw(scene, modelViewTransform_, parentFlags);
     
    scene->popMatrix(MATRIX_STACK_MODELVIEW);
}

void Layer2::draw(RenderEngine::Scene* scene, const Mat4 & transform, uint32_t flags) {
    if (fbo_texture_ && _textureDrawer) {
        
        int64_t duration = scene->getDelta() - _renderRange._start;
        if (_effect_group.has_effect()) {
            SceneNode::drawEffects(duration, scene, transform, fbo_texture_->getTexture());
        }else {
            _textureDrawer->draw(fbo_texture_->getTexture(), scene, transform, this, _flipMode);
        }
    }
}

void Layer2::duplicateDraw(RenderEngine::Scene* scene, const Mat4 & transform, SceneNode* displayNode) {
    if (fbo_texture_ && _textureDrawer) {
        _textureDrawer->draw(fbo_texture_->getTexture(), scene, transform, displayNode, _flipMode);
    }
}

bool Layer2::createRes()
{
    fbo_texture_ = std::unique_ptr<TextureFrameBuffer>(TextureFrameBuffer::createNew(_layerSize.width, _layerSize.height, _enable3d, true));
    Rect svp = Rect(0,0,fbo_texture_->width(),fbo_texture_->height());
    _scene.setViewPort(svp);
    _scene.setProjection(PROJ_3D);

    return SceneNode::createRes();
}

void Layer2::releaseRes() {
    fbo_texture_.reset();
    SceneNode::releaseRes();
}

}
}
