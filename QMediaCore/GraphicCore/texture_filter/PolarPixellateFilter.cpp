//
//  PolarPixellateFilter.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "PolarPixellateFilter.h"
#include "GraphicCore/opengl/shaders/ccShaders.h"
#include "GraphicCore/opengl/Drawable2D.h"

GRAPHICCORE_BEGIN

#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
const char * PolarPixellateFragmentShaderString = R"(
 varying highp vec2 textureCoordinate;
 uniform highp vec2 center;
 uniform highp vec2 pixelSize;

 uniform sampler2D inputImageTexture;

 void main()
 {
     highp vec2 normCoord = 2.0 * textureCoordinate - 1.0;
     highp vec2 normCenter = 2.0 * center - 1.0;

     normCoord -= normCenter;

     highp float r = length(normCoord); // to polar coords
     highp float phi = atan(normCoord.y, normCoord.x); // to polar coords

     r = r - mod(r, pixelSize.x) + 0.03;
     phi = phi - mod(phi, pixelSize.y);

     normCoord.x = r * cos(phi);
     normCoord.y = r * sin(phi);

     normCoord += normCenter;

     mediump vec2 textureCoordinateToUse = normCoord / 2.0 + 0.5;

     gl_FragColor = texture2D(inputImageTexture, textureCoordinateToUse );

 }
)";
#else
const char * PolarPixellateFragmentShaderString = R"(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 
 uniform vec2 center;
 uniform vec2 pixelSize;
 
 
 void main()
 {
     vec2 normCoord = 2.0 * textureCoordinate - 1.0;
     vec2 normCenter = 2.0 * center - 1.0;
     
     normCoord -= normCenter;
     
     float r = length(normCoord); // to polar coords
     float phi = atan(normCoord.y, normCoord.x); // to polar coords
     
     r = r - mod(r, pixelSize.x) + 0.03;
     phi = phi - mod(phi, pixelSize.y);
     
     normCoord.x = r * cos(phi);
     normCoord.y = r * sin(phi);
     
     normCoord += normCenter;
     
     vec2 textureCoordinateToUse = normCoord / 2.0 + 0.5;
     
     gl_FragColor = texture2D(inputImageTexture, textureCoordinateToUse );
     
 }
);
)";
#endif

PolarPixellateFilter::PolarPixellateFilter():_center(Vec2(0.5,0.5)),_pixelSize(Vec2(0.1, 0.1)) {
    
}

bool PolarPixellateFilter::create() {
    bool bRet = _shaderProgram.createProgram(base2dTexture_vert, PolarPixellateFragmentShaderString);
    if (bRet) {
        _shaderProgram.addVertexAttribOption("a_position", VertexAttrib::VERTEX2);
        _shaderProgram.addVertexAttribOption("a_texCoord", VertexAttrib::TEXCOORD);
        _shaderProgram.addUniformOption("inputImageTexture", Uniform::TEXTURE);
        _shaderProgram.addUniformOption("center", Uniform::FLOAT2);
        _shaderProgram.addUniformOption("pixelSize", Uniform::FLOAT2);
    }
    return bRet;
}
void PolarPixellateFilter::drawFilter(int64_t duration, const Texture2D *srcTexture, int flags) {
    if (_shaderProgram.use()) {
        _shaderProgram.setVertexAttribValue("a_position", GET_ARRAY_COUNT(Drawable2D::RECTANGLE_COORDS),Drawable2D::RECTANGLE_COORDS);
        _shaderProgram.setVertexAttribValue("a_texCoord", GET_ARRAY_COUNT(Drawable2D::RECTANGLE_TEX_COORDS),Drawable2D::RECTANGLE_TEX_COORDS);
        
        //set texture uniform
        Uniform::Value texVal;
        texVal._textureTarget = srcTexture->getTextureTarget();
        texVal._texture = srcTexture->getTextureId();
        _shaderProgram.setUniformValue("inputImageTexture",texVal);
        
        //set center uniform
        float center[] = {_center.x, _center.y};
        _shaderProgram.setUniformValue("center",2, center);
        
        //set pixelSize uniform
//        float pixelsize = (float)(duration%200 + 100) / 1000; //test
//        float pixelSize[] = {pixelsize, pixelsize};
        float pixelSize[] = {_pixelSize.x, _pixelSize.y};
        _shaderProgram.setUniformValue("pixelSize",2, pixelSize);
        
        _shaderProgram.drawRect();
    }
}

void PolarPixellateFilter::release() {
    _shaderProgram.releaseProgram();
}

GRAPHICCORE_END
