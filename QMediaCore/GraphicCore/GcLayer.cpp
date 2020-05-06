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
_layerSize(size)
{
    _shaderProgram.enableBlend(true);
}

Layer::~Layer(){
    
}

void Layer::draw(GraphicCore::Scene* scene, const Mat4 & transform, uint32_t flags)
{
    duplicateDraw(scene, transform, this);
}

void Layer::visit(GraphicCore::Scene *scene, const Mat4& parentTransform, uint32_t parentFlags)
{
    Range<int64_t> dispRange = getRange();
    if (scene->getDelta() < dispRange._start || scene->getDelta() > dispRange._end)
        return; //isn't in display time range
    
    if (!_bVisible)
    {
        return;
    }
    
    if (_transformDirty || _contentSizeDirty) {
        _modelViewTransform = Node::transform(parentTransform);
    }
    _transformUpdated = false;
    _contentSizeDirty = false;

    scene->pushMatrix(MATRIX_STACK_MODELVIEW);
    scene->loadMatrix(MATRIX_STACK_MODELVIEW, _modelViewTransform);
    
    if(!_children.empty()){
        //update timestamp
        _scene.setDelta(scene->getDelta());
        _scene.setGLEngine(scene->getGLEngine());
        //draw children with self's scene
        Rect layerViewPort(0, 0,_texture2d->width(),_texture2d->height()) , prevViewPort;
        GLEngine *gle = _scene.getGLEngine();

        gle->saveStatus();
        gle->setCurrentFrameBuffer(_framebuffer);
        gle->setViewPort(layerViewPort);
        
        Mat4 layerTransform;
        gle->setClearColor(_bkColor);
        gle->clearByColor();
        for (auto& node : _children) {
            node->visit(&_scene, layerTransform, parentFlags);
        }

        gle->recoverStatus();
    }

    this->draw(scene, _modelViewTransform, parentFlags);
    
    scene->popMatrix(MATRIX_STACK_MODELVIEW);
}

void Layer::duplicateDraw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* diaplayNode)
{
    //draw with parent's scene
    //FIXME: translation of position already contain in transform matrix
    VertexAttrib::Value vertValue;
    vertValue._buffer = {
        0, 0 , diaplayNode->getPositionZ(),
        diaplayNode->getContentSize().width , 0 , diaplayNode->getPositionZ(),
        0 , diaplayNode->getContentSize().height , diaplayNode->getPositionZ(),
        diaplayNode->getContentSize().width , diaplayNode->getContentSize().height, diaplayNode->getPositionZ(),
    };
    
    Mat4 mvpMatrix, texMatrix;
    Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
    
    if(_shaderProgram.use()){
        Uniform::Value colorVal;
        Color4F realColor = diaplayNode->getColor();
        colorVal._floatOrmatrix_array = {realColor.r,realColor.g,realColor.b,realColor.a};
        _shaderProgram.setUniformValue("uColor", colorVal);
        _shaderProgram.setVertexAttribValue("a_position", vertValue);
        _shaderProgram.setVertexAttribValue("a_texCoord", 8, Drawable2D::RECTANGLE_TEX_COORDS);
        _shaderProgram.setUniformValue("uMVPMatrix",GET_ARRAY_COUNT(mvpMatrix.m),mvpMatrix.m);
        _shaderProgram.setUniformValue("uTexMatrix",GET_ARRAY_COUNT(texMatrix.m),texMatrix.m);
        _shaderProgram.drawRect();
    }
}

void Layer::addFilter(FilterRef filter){
    
}

bool Layer::createRes()
{
    bool bRet = false;
    _texture2d = GeneralTexture2D::createTexture(Image::RGBA, _layerSize.width,_layerSize.height);
    Rect svp = Rect(0,0,_texture2d->width(),_texture2d->height());
    _scene.setViewPort(svp);
    _scene.setProjection(Projection::_3D);
    _framebuffer = FrameBuffer::createNew();
    _framebuffer->attachTexture2D(FrameBuffer::COLOR, _texture2d);
    if(_shaderProgram.createProgram(defaultPositionTexture_vert,
                                    defaultPositionTexture_frag)){
        _shaderProgram.addVertexAttribOption("a_position",VertexAttrib::VERTEX3);
        _shaderProgram.addVertexAttribOption("a_texCoord",VertexAttrib::TEXCOORD);
        _shaderProgram.addUniformOption("uMVPMatrix",Uniform::MATRIX4);
        _shaderProgram.addUniformOption("uTexMatrix",Uniform::MATRIX4);
        _shaderProgram.addUniformOption("uTexture",Uniform::TEXTURE);
        _shaderProgram.addUniformOption("uColor",Uniform::FLOAT4);
//        _shaderProgram.setProgramLocations();
        
        Uniform::Value texVal;
        texVal._texture = _texture2d->getTextureId();
        _shaderProgram.setUniformValue("uTexture",texVal);
        bRet = true;
    }
    RenderNode::createRes();
    return bRet;
}

void Layer::releaseRes()
{
    _shaderProgram.releaseProgram();
    if (_framebuffer) {
        _framebuffer->attachTexture2D(FrameBuffer::COLOR, nullptr);
        _framebuffer->release();
        delete _framebuffer;
    }
    if (_texture2d) {
        _texture2d->release();
        delete _texture2d;
    }
    RenderNode::releaseRes();
}

GRAPHICCORE_END
