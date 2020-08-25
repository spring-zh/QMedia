//
//  SobelEdgeDetectionConfig.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FilterConfig.h"
#include "GraphicCore/opengl/Drawable2D.h"

#define FILTERNAME "SobelEdgeDetection"

GRAPHICCORE_BEGIN

const char* SobelEdgeDetectionVertexShaderString = R"(
attribute vec4 position;
attribute vec4 inputTextureCoordinate;

uniform float texelWidth;
uniform float texelHeight;

varying vec2 textureCoordinate;
varying vec2 leftTextureCoordinate;
varying vec2 rightTextureCoordinate;

varying vec2 topTextureCoordinate;
varying vec2 topLeftTextureCoordinate;
varying vec2 topRightTextureCoordinate;

varying vec2 bottomTextureCoordinate;
varying vec2 bottomLeftTextureCoordinate;
varying vec2 bottomRightTextureCoordinate;

void main()
{
    gl_Position = position;
    
    vec2 widthStep = vec2(texelWidth, 0.0);
    vec2 heightStep = vec2(0.0, texelHeight);
    vec2 widthHeightStep = vec2(texelWidth, texelHeight);
    vec2 widthNegativeHeightStep = vec2(texelWidth, -texelHeight);
    
    textureCoordinate = inputTextureCoordinate.xy;
    leftTextureCoordinate = inputTextureCoordinate.xy - widthStep;
    rightTextureCoordinate = inputTextureCoordinate.xy + widthStep;
    
    topTextureCoordinate = inputTextureCoordinate.xy - heightStep;
    topLeftTextureCoordinate = inputTextureCoordinate.xy - widthHeightStep;
    topRightTextureCoordinate = inputTextureCoordinate.xy + widthNegativeHeightStep;
    
    bottomTextureCoordinate = inputTextureCoordinate.xy + heightStep;
    bottomLeftTextureCoordinate = inputTextureCoordinate.xy - widthNegativeHeightStep;
    bottomRightTextureCoordinate = inputTextureCoordinate.xy + widthHeightStep;
}
)";

const char * SobelEdgeDetectionFragmentShaderString = R"(
#ifdef GL_ES
 precision mediump float;
#endif
 
 varying vec2 textureCoordinate;
 varying vec2 leftTextureCoordinate;
 varying vec2 rightTextureCoordinate;
 
 varying vec2 topTextureCoordinate;
 varying vec2 topLeftTextureCoordinate;
 varying vec2 topRightTextureCoordinate;
 
 varying vec2 bottomTextureCoordinate;
 varying vec2 bottomLeftTextureCoordinate;
 varying vec2 bottomRightTextureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform float edgeStrength;

 void main()
 {
     float bottomLeftIntensity = texture2D(inputImageTexture, bottomLeftTextureCoordinate).r;
     float topRightIntensity = texture2D(inputImageTexture, topRightTextureCoordinate).r;
     float topLeftIntensity = texture2D(inputImageTexture, topLeftTextureCoordinate).r;
     float bottomRightIntensity = texture2D(inputImageTexture, bottomRightTextureCoordinate).r;
     float leftIntensity = texture2D(inputImageTexture, leftTextureCoordinate).r;
     float rightIntensity = texture2D(inputImageTexture, rightTextureCoordinate).r;
     float bottomIntensity = texture2D(inputImageTexture, bottomTextureCoordinate).r;
     float topIntensity = texture2D(inputImageTexture, topTextureCoordinate).r;
     float h = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0 * bottomIntensity + bottomRightIntensity;
     float v = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;
     
     float mag = length(vec2(h, v)) * edgeStrength;
     
     gl_FragColor = vec4(vec3(mag), 1.0);
 }
)";

const char * SobelEdgeDetectionDescribe = R"({ "edgeStrength": {"type":"FLOAT", "default": 1.f, "max": 2.0 , "min": 0.0} })";

FilterConfigRef createSobelEdgeDetectionConfig() {
    
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig(FILTERNAME, SobelEdgeDetectionDescribe));
    config->vertex_sh = SobelEdgeDetectionVertexShaderString;
    config->fragment_sh = SobelEdgeDetectionFragmentShaderString;
    
    //vertex value
    FilterConfig::VertexConfig positionVC;
    positionVC.key = "position";
    positionVC.type = VertexAttrib::VERTEX2;
    positionVC.value.copyForm(Drawable2D::RECTANGLE_COORDS, 8);
    
    FilterConfig::VertexConfig texCoordVC;
    texCoordVC.key = "inputTextureCoordinate";
    texCoordVC.type = VertexAttrib::VERTEX2;
    texCoordVC.value.copyForm(Drawable2D::RECTANGLE_TEX_COORDS, 8);
    
    //uniform value
    
    FilterConfig::UniformConfig widthUC;
    widthUC.key = "texelWidth";
    widthUC.type = Uniform::FLOAT;
    widthUC.value._float_val = 0.001f;
    
    FilterConfig::UniformConfig heightUC;
    heightUC.key = "texelHeight";
    heightUC.type = Uniform::FLOAT;
    heightUC.value._float_val = 0.001f;
    
    FilterConfig::UniformConfig edgeStrengthUC;
    edgeStrengthUC.key = "edgeStrength";
    edgeStrengthUC.type = Uniform::FLOAT;
    edgeStrengthUC.value._float_val = 1.f;
    
    config->_vertexConfig.push_back(std::move(positionVC));
    config->_vertexConfig.push_back(std::move(texCoordVC));
    config->_uniformConfigs.push_back(std::move(widthUC));
    config->_uniformConfigs.push_back(std::move(heightUC));
    config->_uniformConfigs.push_back(std::move(edgeStrengthUC));
    
    config->inputTexture = "inputTexture";
    return config;
}

GRAPHICCORE_END
