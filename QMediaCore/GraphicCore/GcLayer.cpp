//
//  GcLayer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcLayer.h"
#include "Utils/Comm.h"
#include "opengl/Drawable2D.h"
#include "opengl/shaders/ccShaders.h"

GRAPHICCORE_BEGIN

Layer::Layer(Size size):
_framebuffer(nullptr),
_texture(nullptr),
_bkColor(0,0,0,1),
_layerSize(size),
_enable3d(false){
}

Layer::~Layer(){
    
}

void Layer::visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags)
{
    Range<int64_t> dispRange = getRange();
    if ( dispRange.isValid() && (scene->getDelta() < dispRange._start || scene->getDelta() > dispRange._end))
        return; //isn't in display time range

    AnimaNode::updateAnimations(scene->getDelta() - getRange()._start);
    
    if (!_bVisible)
    {
        return;
    }
    
    if (_transformDirty || _contentSizeDirty || (parentFlags & Flags_Update) ) {
        _modelViewTransform = Node::transform(parentTransform);
    }
    _transformUpdated = false;
    _contentSizeDirty = false;

    scene->pushMatrix(MATRIX_STACK_MODELVIEW);
    scene->loadMatrix(MATRIX_STACK_MODELVIEW, _modelViewTransform);

    GLEngine *gle = scene->getGLEngine();
    gle->saveStatus();
    bool isDepthTest = gle->enableDepthTest();
    gle->setEnableDepthTest(_enable3d);
    // draw layer
    if ((int)_layerSize.width != _texture->width() || (int)_layerSize.height != _texture->height()) {
        //TODO: reset layer size
        _texture.reset();
        _texture = std::shared_ptr<Texture2D>(GeneralTexture2D::createTexture(Texture2D::RGBA, _layerSize.width,_layerSize.height));
    }
    gle->setCurrentFrameBuffer(_framebuffer.get());
    Rect layerViewPort(0, 0,_texture->width(),_texture->height());
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
    gle->recoverStatus();
    gle->setEnableDepthTest(isDepthTest);
    this->draw(scene, _modelViewTransform, parentFlags);
    
    scene->popMatrix(MATRIX_STACK_MODELVIEW);
}

void Layer::draw(GraphicCore::Scene* scene, const Mat4 & transform, uint32_t flags) {
    if (_texture && _textureDrawer) {
        
        int64_t duration = scene->getDelta() - _renderRange._start;
        if (_effect_group.has_effect()) {
            RenderNode::drawEffects(duration, scene, transform, _texture.get());
        }else {
            _textureDrawer->draw(_texture.get(), scene, transform, this, _flipMode);
        }
    }
}

void Layer::duplicateDraw(GraphicCore::Scene* scene, const Mat4 & transform, const Node* displayNode)
{
    if (_texture && _textureDrawer) {
        _textureDrawer->draw(_texture.get(), scene, transform, displayNode, _flipMode);
    }
}

bool Layer::createRes()
{
    _texture = std::shared_ptr<Texture2D>(GeneralTexture2D::createTexture(Texture2D::RGBA, _layerSize.width,_layerSize.height));
    Rect svp = Rect(0,0,_texture->width(),_texture->height());
    _scene.setViewPort(svp);
    _scene.setProjection(Projection::_3D);
    _framebuffer = std::shared_ptr<FrameBuffer>(FrameBuffer::createNew());
    _framebuffer->attachTexture2D(FrameBuffer::COLOR, _texture.get());
    if (_enable3d) {
        _depth_texture = std::unique_ptr<Texture2D>(GeneralTexture2D::createTexture(Texture2D::DEPTH, _layerSize.width,_layerSize.height));
        _framebuffer->attachTexture2D(FrameBuffer::DEPTH, _depth_texture.get());
    }

    return RenderNode::createRes();
}

void Layer::releaseRes()
{
    _framebuffer.reset();
    _texture.reset();
    _depth_texture.reset();
    
    RenderNode::releaseRes();
}

GRAPHICCORE_END
