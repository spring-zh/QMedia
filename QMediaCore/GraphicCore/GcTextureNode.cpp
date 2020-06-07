//
//  GcTextureNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcTextureNode.h"
#include "Utils/Comm.h"
#include "opengl/Drawable2D.h"
#include "opengl/shaders/ccShaders.h"

GRAPHICCORE_BEGIN

TextureNode::TextureNode(): _texture2d(nullptr) {
}

TextureNode::~TextureNode(){
    
}

//call by DuplicateNode
void TextureNode::duplicateDraw(Scene* scene, const Mat4 & transform, const Node* diaplayNode)
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
            //set texture uniform
            Uniform::Value texVal;
            texVal._textureTarget = _texture2d->getTextureTarget();
            texVal._texture = _texture2d->getTextureId();
            _shaderProgram.setUniformValue("uTexture",texVal);

            //set color uniform
            Uniform::Value colorVal;
            Color4F realColor = diaplayNode->getColor();
            colorVal._floatOrmatrix_array = {realColor.r,realColor.g,realColor.b,realColor.a};
            _shaderProgram.setUniformValue("uColor", colorVal);
            if (! FLOAT_ISEQUAL(diaplayNode->getColor().a, 1.0f)) {
                _shaderProgram.enableBlend(true);
            }
            else
                _shaderProgram.enableBlend(false);

            _shaderProgram.setVertexAttribValue("a_position", vertValue);
            _shaderProgram.setVertexAttribValue("a_texCoord", 8, Drawable2D::RECTANGLE_TEX_COORDS);
            _shaderProgram.setUniformValue("uMVPMatrix",GET_ARRAY_COUNT(mvpMatrix.m),mvpMatrix.m);
            _shaderProgram.setUniformValue("uTexMatrix",GET_ARRAY_COUNT(texMatrix.m),texMatrix.m);
            _shaderProgram.drawRect();
        }
}

bool TextureNode::createRes() {
    //TODO:
    bool bRet = false;
    if(_shaderProgram.createProgram(defaultPositionTexture_vert,
                                    defaultPositionTexture_frag)){
        _shaderProgram.addVertexAttribOption("a_position",VertexAttrib::VERTEX3);
        _shaderProgram.addVertexAttribOption("a_texCoord",VertexAttrib::TEXCOORD);
        _shaderProgram.addUniformOption("uMVPMatrix",Uniform::MATRIX4);
        _shaderProgram.addUniformOption("uTexMatrix",Uniform::MATRIX4);
        _shaderProgram.addUniformOption("uTexture",Uniform::TEXTURE);
        _shaderProgram.addUniformOption("uColor",Uniform::FLOAT4);
        bRet = true;
    }
    return bRet && RenderNode::createRes();
}
void TextureNode::releaseRes() {
    //TODO:
    _shaderProgram.releaseProgram();
    RenderNode::releaseRes();
}

GRAPHICCORE_END
