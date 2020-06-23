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
_texture_first(nullptr),_texture_second(nullptr),
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
    Rect layerViewPort(0, 0,_texture_first->width(),_texture_first->height());
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

void Layer::draw(GraphicCore::Scene* scene, const Mat4 & transform, uint32_t flags) {
    #define NEXT_TEXTURE (current==_texture_first? _texture_second : _texture_first)
    if (_texture_first && _textureDrawer) {
        
        int64_t duration = scene->getDelta() - _renderRange._start;
        Texture2D* current = _texture_first;
        
        if (_effect_group.size() > 0) {
            GLEngine *gle = scene->getGLEngine();
            gle->saveStatus();
            //TODO: draw filters
            if (_texture_second == nullptr) {
                _texture_second = GeneralTexture2D::createTexture(Texture2D::RGBA, _layerSize.width,_layerSize.height);
            }
            for (auto& effect : _effect_group) {
                if (effect->_timeRange.isContain(duration)) {
                    Texture2D * next = NEXT_TEXTURE;
                    gle->setCurrentFrameBuffer(_framebuffer);
                    _framebuffer->attachTexture2D(FrameBuffer::COLOR, next);
                    Rect layerViewPort(0, 0,next->width(),next->height());
                    gle->setViewPort(layerViewPort);
                    effect->drawEffect(duration, current, gle);
                    current = next;
                }
            }
            gle->recoverStatus();
        }
        
        _textureDrawer->draw(current, scene, transform, this);
    }
}

void Layer::duplicateDraw(GraphicCore::Scene* scene, const Mat4 & transform, const Node* displayNode)
{
    if (_texture_first && _textureDrawer) {
        _textureDrawer->draw(_texture_first, scene, transform, displayNode);
    }
}

void Layer::addEffect(EffectRef effectRef) {
    _effect_group.push_back(effectRef);
}
void Layer::removeEffect(EffectRef effectRef) {
    std::remove(_effect_group.begin(), _effect_group.end(), effectRef);
}

bool Layer::createRes()
{
    _texture_first = GeneralTexture2D::createTexture(Texture2D::RGBA, _layerSize.width,_layerSize.height);
    Rect svp = Rect(0,0,_texture_first->width(),_texture_first->height());
    _scene.setViewPort(svp);
    _scene.setProjection(Projection::_3D);
    _framebuffer = FrameBuffer::createNew();
    _framebuffer->attachTexture2D(FrameBuffer::COLOR, _texture_first);
    _textureDrawer = std::shared_ptr<Texture2DDrawer>(new Texture2DDrawer());

    return RenderNode::createRes();
}

void Layer::releaseRes()
{
    if (_framebuffer) {
//        _framebuffer->attachTexture2D(FrameBuffer::COLOR, nullptr);
        _framebuffer->release();
        delete _framebuffer;
    }
    if (_texture_first) {
        _texture_first->release();
        delete _texture_first;
        _texture_first = nullptr;
    }
    if (_texture_second) {
        _texture_second->release();
        delete _texture_second;
        _texture_second = nullptr;
    }
    _textureDrawer.reset();
    RenderNode::releaseRes();
}

GRAPHICCORE_END
