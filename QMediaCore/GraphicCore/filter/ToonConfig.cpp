//
//  PolarPixellateConfig.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FilterConfig.h"
#include "GraphicCore/opengl/Drawable2D.h"

#define FILTERNAME "Toon"

GRAPHICCORE_BEGIN

const char* ToonVertexShaderString = R"(
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

#if TARGET_PLATFORM == PLATFORM_IOS || TARGET_PLATFORM == PLATFORM_ANDROID
const char * ToonFragmentShaderString = R"(
 precision highp float;
 
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
 
 uniform highp float intensity;
 uniform highp float threshold;
 uniform highp float quantizationLevels;
 
 const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);
 
 void main()
 {
     vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     
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
     
     float mag = length(vec2(h, v));

     vec3 posterizedImageColor = floor((textureColor.rgb * quantizationLevels) + 0.5) / quantizationLevels;
     
     float thresholdTest = 1.0 - step(threshold, mag);
     
     gl_FragColor = vec4(posterizedImageColor * thresholdTest, textureColor.a);
 }
)";
#else
const char * ToonFragmentShaderString = R"(
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
 
 uniform float intensity;
 uniform float threshold;
 uniform float quantizationLevels;
 
 const vec3 W = vec3(0.2125, 0.7154, 0.0721);
 
 void main()
 {
     vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     
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
     
     float mag = length(vec2(h, v));
     
     vec3 posterizedImageColor = floor((textureColor.rgb * quantizationLevels) + 0.5) / quantizationLevels;
     
     float thresholdTest = 1.0 - step(threshold, mag);
     
     gl_FragColor = vec4(posterizedImageColor * thresholdTest, textureColor.a);
 }
)";
#endif

//const char * ToonDescribe = R"({ "threshold": {"type":"FLOAT", "default": 0.2, "max": 1.0 , "min": 0.0} })";
const char * ToonDescribe = "";

FilterConfigRef createToonConfig() {
    
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig(FILTERNAME, ToonDescribe));
    config->vertex_sh = ToonVertexShaderString;
    config->fragment_sh = ToonFragmentShaderString;
    
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
    
    FilterConfig::UniformConfig intensityUC;
    intensityUC.key = "intensity";
    intensityUC.type = Uniform::FLOAT;
    intensityUC.value._float_val = 0;
    
    FilterConfig::UniformConfig thresholdUC;
    thresholdUC.key = "threshold";
    thresholdUC.type = Uniform::FLOAT;
    thresholdUC.value._float_val = 0.2f;
    
    FilterConfig::UniformConfig quantizationLevelsUC;
    quantizationLevelsUC.key = "quantizationLevels";
    quantizationLevelsUC.type = Uniform::FLOAT;
    quantizationLevelsUC.value._float_val = 10.0f;
    
    config->_vertexConfig.push_back(std::move(positionVC));
    config->_vertexConfig.push_back(std::move(texCoordVC));
    config->_uniformConfigs.push_back(std::move(widthUC));
    config->_uniformConfigs.push_back(std::move(heightUC));
    config->_uniformConfigs.push_back(std::move(intensityUC));
    config->_uniformConfigs.push_back(std::move(thresholdUC));
    config->_uniformConfigs.push_back(std::move(quantizationLevelsUC));
    
    config->inputTexture = "inputTexture";
    return config;
}

GRAPHICCORE_END
