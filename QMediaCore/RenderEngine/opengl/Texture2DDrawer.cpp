//
//  Texture2DDrawer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Utils/Comm.h"
#include "RenderEngine/opengl/shader_code.h"
#include "RenderEngine/SceneNode.h"
#include "Texture2DDrawer.h"

namespace QMedia {
namespace RenderEngine {

Texture2DDrawer::Texture2DDrawer(): Texture2DDrawer(defaultPositionTexture_vert, defaultPositionTexture_frag){
}

Texture2DDrawer::Texture2DDrawer(const char* vshader, const char* fshader) {
    _shaderProgram.createProgram(vshader, fshader);
}

Texture2DDrawer::~Texture2DDrawer() {
    _shaderProgram.releaseProgram();
}

//void Texture2DDrawer::draw(const Texture2D* texture,const Scene* scene, const Mat4 & transform, const Node* node, Drawable2D::FlipMode flipMode ) {
//    Mat4 mvpMatrix;
//    Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
//    //FIXME: translation of position already contain in transform matrix
//    GraphicCore::Rect region(Vec2(0,0), node->getContentSize());
//    draw(texture, mvpMatrix, region, node->getPositionZ(),node->getCrop(),node->getColor(),node->getBlendFunc(), flipMode);
//}

void Texture2DDrawer::draw(const Texture2D* texture,const Scene* scene, const Mat4 & transform, Api::SceneNode* node, Drawable2D::FlipMode flipMode) {
    Mat4 mvpMatrix;
    Mat4::multiply(scene->getMatrix(Api::MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
    RenderEngine::Rect region(0,0, node->getContentSize().width, node->getContentSize().height);
    auto crop_v = node->getCrop();
    RenderEngine::Rect crop(crop_v.left, crop_v.top, crop_v.right, crop_v.bottom);
    auto color_v = node->getColor4F();
    RenderEngine::Color4F color = RenderEngine::Color4F(color_v.r, color_v.g, color_v.b, color_v.a);
    RenderEngine::BlendFunc bf = BlendFunc::DISABLE;
    
    draw(texture, mvpMatrix, region, node->getPositionZ(),crop,color,bf, flipMode);
}

void Texture2DDrawer::draw(const Texture2D* texture, const Mat4& mvpMatrix, const RenderEngine::Rect & region, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
                                 const BlendFunc& blend, Drawable2D::FlipMode flipMode) {
    VertexAttrib::Vec3<float> vertValue[] = {
        {region.getMinX(), region.getMinY(), positionZ},
        {region.getMaxX(), region.getMinY(), positionZ},
        {region.getMinX(), region.getMaxY(), positionZ},
        {region.getMaxX(), region.getMaxY(), positionZ}
    };

    if(_shaderProgram.use()){

        if (blend != BlendFunc::DISABLE) {
            _shaderProgram.setBlendFunc(blend);
        } else if ( FLOAT_ISEQUAL(color.a, 1.0f)){
            _shaderProgram.setBlendFunc(BlendFunc::DISABLE);
        } else
            _shaderProgram.setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);

        //set texture uniform
        Uniform::TextureUnit tex_val(texture->getTextureId(), texture->getTextureTarget());
        _shaderProgram.setUniformValue("uTexture", tex_val);

        //set color uniform
        Uniform::Vec4<float> color_val = {color.r,color.g,color.b,color.a};
        _shaderProgram.setUniformValue("uColor", color_val);
        _shaderProgram.setUniformValue("uMVPMatrix", Uniform::Matrix(mvpMatrix.m, 4 ,1));

        _shaderProgram.setVertexAttribValue("a_position", vertValue, 4);
        _shaderProgram.setVertexAttribValue("a_texCoord", (VertexAttrib::Vec2<float>*)(Drawable2D::RECTANGLE_TEX_COORDS),4);

        Mat4 texMatrix;
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

        _shaderProgram.setUniformValue("uTexMatrix", Uniform::Matrix(texMatrix.m, 4, 1));
        _shaderProgram.drawRectangle();
    }
}

}
}
