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
    _shaderProgram.createProgram(vshader, fshader);
}

Texture2DDrawer::~Texture2DDrawer() {
    _shaderProgram.releaseProgram();
}

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
        _shaderProgram.setUniformValue("uTexture", Uniform::TEXTURE, texVal);

        //set color uniform
        Uniform::Value colorVal;
        Color4F realColor = node->getColor();
        colorVal._floatOrmatrix_array = {realColor.r,realColor.g,realColor.b,realColor.a};
        _shaderProgram.setUniformValue("uColor", Uniform::FLOAT4 ,colorVal);
        _shaderProgram.setUniformValue("uMVPMatrix", Uniform::MATRIX4 ,mvpMatrix.m, GET_ARRAY_COUNT(mvpMatrix.m));

        _shaderProgram.setVertexAttribValue("a_position", VertexAttrib::VERTEX3, vertValue);
        _shaderProgram.setVertexAttribValue("a_texCoord", VertexAttrib::VERTEX2, Drawable2D::RECTANGLE_TEX_COORDS, 8);

        //TODO: check filp mode
        switch (flipMode) {
            case Drawable2D::FlipH:
                texMatrix = Drawable2D::MtxFlipH;
                break;
            case Drawable2D::FlipV:
                texMatrix = Drawable2D::MtxFlipV;
                break;
            default:
                break;
        }

        //TODO: check crop
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

        _shaderProgram.setUniformValue("uTexMatrix", Uniform::MATRIX4,texMatrix.m, GET_ARRAY_COUNT(texMatrix.m));
        _shaderProgram.drawRectangle();
    }
}

void Texture2DDrawer::drawDirect(const Texture2D* texture,const GraphicCore::Scene* scene, const GraphicCore::Rect & region, const GraphicCore::Rect crop, GraphicCore::Color4F color, GraphicCore::Drawable2D::FlipMode flipMode) {
    VertexAttrib::Value vertValue;
    vertValue._buffer = {
            region.getMinX(), region.getMinY(), 0,
            region.getMaxX(), region.getMinY(), 0,
            region.getMinX(), region.getMaxY(), 0,
            region.getMaxX(), region.getMaxY(), 0
    };

    Mat4 mvpMatrix = scene->getMatrix(MATRIX_STACK_PROJECTION), texMatrix;
//    Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);

    if(_shaderProgram.use()){

        if ( FLOAT_ISEQUAL(color.a, 1.0f)){
            _shaderProgram.setBlendFunc(BlendFunc::DISABLE);
        } else
            _shaderProgram.setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);

        //set texture uniform
        Uniform::Value texVal;
        texVal._textureTarget = texture->getTextureTarget();
        texVal._texture = texture->getTextureId();
        _shaderProgram.setUniformValue("uTexture", Uniform::TEXTURE, texVal);

        //set color uniform
        Uniform::Value colorVal;
        colorVal._floatOrmatrix_array = {color.r,color.g,color.b,color.a};
        _shaderProgram.setUniformValue("uColor", Uniform::FLOAT4 ,colorVal);
        _shaderProgram.setUniformValue("uMVPMatrix", Uniform::MATRIX4 ,mvpMatrix.m, GET_ARRAY_COUNT(mvpMatrix.m));

        _shaderProgram.setVertexAttribValue("a_position", VertexAttrib::VERTEX3, vertValue);
        _shaderProgram.setVertexAttribValue("a_texCoord", VertexAttrib::VERTEX2, Drawable2D::RECTANGLE_TEX_COORDS, 8);

        //TODO: check filp mode
        switch (flipMode) {
            case Drawable2D::FlipH:
                texMatrix = Drawable2D::MtxFlipH;
                break;
            case Drawable2D::FlipV:
                texMatrix = Drawable2D::MtxFlipV;
                break;
            default:
                break;
        }

        //TODO: check crop
        if (! crop.size.equals(Size::ZERO)) {
            //TODO: need crop
            float crop_arr[16] = {
                    crop.size.width, 0, 0, 0,
                    0, crop.size.height, 0, 0,
                    0, 0, 1, 0,
                    crop.getMinX(), crop.getMinY(), 0, 1,
            };
            texMatrix.multiply(crop_arr);
        }

        _shaderProgram.setUniformValue("uTexMatrix", Uniform::MATRIX4,texMatrix.m, GET_ARRAY_COUNT(texMatrix.m));
        _shaderProgram.drawRectangle();
    }
}

GRAPHICCORE_END
