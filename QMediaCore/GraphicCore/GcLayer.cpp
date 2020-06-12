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
_texture2d(nullptr),
_bkColor(0,0,0,1),
_layerSize(size) {
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
    // draw layer
    gle->setCurrentFrameBuffer(_framebuffer);
    Rect layerViewPort(0, 0,_texture2d->width(),_texture2d->height()) , prevViewPort;
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

    this->draw(scene, _modelViewTransform, parentFlags);
    
    scene->popMatrix(MATRIX_STACK_MODELVIEW);
}

void Layer::duplicateDraw(GraphicCore::Scene* scene, const Mat4 & transform, const Node* displayNode) {
    if (_texture2d && _textureDrawer) {
        _textureDrawer->draw(_texture2d, scene, transform, displayNode);
    }
}

void Layer::addFilter(FilterRef filter){
    
}

bool Layer::createRes()
{
    _texture2d = GeneralTexture2D::createTexture(Texture2D::RGBA, _layerSize.width,_layerSize.height);
    Rect svp = Rect(0,0,_texture2d->width(),_texture2d->height());
    _scene.setViewPort(svp);
    _scene.setProjection(Projection::_3D);
    _framebuffer = FrameBuffer::createNew();
    _framebuffer->attachTexture2D(FrameBuffer::COLOR, _texture2d);
    _textureDrawer = std::shared_ptr<Texture2DDrawer>(new Texture2DDrawer());

    return RenderNode::createRes();
}

void Layer::releaseRes()
{
//    _shaderProgram.releaseProgram();
    if (_framebuffer) {
//        _framebuffer->attachTexture2D(FrameBuffer::COLOR, nullptr);
        _framebuffer->release();
        delete _framebuffer;
    }
    if (_texture2d) {
        _texture2d->release();
        delete _texture2d;
        _texture2d = nullptr;
    }
    _textureDrawer.reset();
    RenderNode::releaseRes();
}

GRAPHICCORE_END
