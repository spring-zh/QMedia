//
//  Texture2DDrawer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Texture2DDrawer.h"
#include "Utils/Comm.h"
#include "shaders/ccShaders.h"

GRAPHICCORE_BEGIN

Texture2DDrawer::Texture2DDrawer(): Texture2DDrawer(defaultPositionTexture_vert, defaultPositionTexture_frag){
}

Texture2DDrawer::Texture2DDrawer(const char* vshader, const char* fshader) {
    if(_shaderProgram.createProgram(vshader,
                                    fshader)){
        _shaderProgram.addVertexAttribOption("a_position",VertexAttrib::VERTEX3);
        _shaderProgram.addVertexAttribOption("a_texCoord",VertexAttrib::TEXCOORD);
        _shaderProgram.addUniformOption("uMVPMatrix",Uniform::MATRIX4);
        _shaderProgram.addUniformOption("uTexMatrix",Uniform::MATRIX4);
        _shaderProgram.addUniformOption("uTexture",Uniform::TEXTURE);
        _shaderProgram.addUniformOption("uColor",Uniform::FLOAT4);
    }
}

Texture2DDrawer::~Texture2DDrawer() {
    _shaderProgram.releaseProgram();
}

//call by DuplicateNode
void Texture2DDrawer::draw(const Texture2D* texture,const Scene* scene, const Mat4 & transform, const Node* node, Drawable2D::FlipMode flipMode ) {
    //FIXME: translation of position already contain in transform matrix
    VertexAttrib::Value vertValue;
    vertValue._buffer = {
            0, 0 , node->getPositionZ(),
            node->getContentSize().width , 0 , node->getPositionZ(),
            0 , node->getContentSize().height , node->getPositionZ(),
            node->getContentSize().width , node->getContentSize().height, node->getPositionZ(),
    };

    Mat4 mvpMatrix, texMatrix;
    Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);

    if(_shaderProgram.use()){

        if (node->getBlendFunc() != BlendFunc::DISABLE) {
            _shaderProgram.setBlendFunc(node->getBlendFunc());
        } else if ( FLOAT_ISEQUAL(node->getColor().a, 1.0f)){
            _shaderProgram.setBlendFunc(BlendFunc::DISABLE);
        } else
            _shaderProgram.setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);

        //set texture uniform
        Uniform::Value texVal;
        texVal._textureTarget = texture->getTextureTarget();
        texVal._texture = texture->getTextureId();
        _shaderProgram.setUniformValue("uTexture",texVal);

        //set color uniform
        Uniform::Value colorVal;
        Color4F realColor = node->getColor();
        colorVal._floatOrmatrix_array = {realColor.r,realColor.g,realColor.b,realColor.a};
        _shaderProgram.setUniformValue("uColor", colorVal);
        _shaderProgram.setUniformValue("uMVPMatrix",GET_ARRAY_COUNT(mvpMatrix.m),mvpMatrix.m);

        _shaderProgram.setVertexAttribValue("a_position", vertValue);
        _shaderProgram.setVertexAttribValue("a_texCoord", 8, Drawable2D::RECTANGLE_TEX_COORDS);

        //TODO: check filp mode
        switch (flipMode) {
            case Drawable2D::FlipH:
                texMatrix = Drawable2D::MtxFlipH;
                break;
            case Drawable2D::FlipV:
                texMatrix = Drawable2D::MtxFlipV;
                break;
        }

        Rect cropRect = node->getCrop();
        if (! cropRect.size.equals(Size::ZERO)) {
            //TODO: need crop
            float crop[16] = {
                    cropRect.size.width, 0, 0, 0,
                    0, cropRect.size.height, 0, 0,
                    0, 0, 1, 0,
                    cropRect.getMinX(), cropRect.getMinY(), 0, 1,
            };
            texMatrix.multiply(crop);
        }

        _shaderProgram.setUniformValue("uTexMatrix",GET_ARRAY_COUNT(texMatrix.m),texMatrix.m);
        _shaderProgram.drawRect();
    }
}

GRAPHICCORE_END
