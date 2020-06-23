//
//  FilterConfig.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FilterConfig.h"
#include "GraphicCore/opengl/Drawable2D.h"

GRAPHICCORE_BEGIN

const char* DefaultFilterVertexShaderString = R"(
attribute vec4 a_position;
attribute vec4 a_texCoord;

#ifdef GL_ES
varying highp vec2 textureCoordinate;
#else
varying vec2 textureCoordinate;
#endif

void main()
{
    gl_Position = a_position;
    textureCoordinate = a_texCoord.xy;
}
)";

const char * LuminanceFragmentShaderString = R"(
#ifdef GL_ES
  precision highp float;
#endif
  
  varying vec2 textureCoordinate;
  
  uniform sampler2D inputImageTexture;
  
  const vec3 W = vec3(0.2125, 0.7154, 0.0721);
  
  void main()
  {
      vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      float luminance = dot(textureColor.rgb, W);
      
      gl_FragColor = vec4(vec3(luminance), textureColor.a);
  }
)";

const char * LuminanceThresholdFragmentShaderString = R"(
#ifdef GL_ES
  precision highp float;
#endif
  varying vec2 textureCoordinate;
  
  uniform sampler2D inputImageTexture;
  uniform float threshold;
  
  const vec3 W = vec3(0.2125, 0.7154, 0.0721);
  
  void main()
  {
      vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      float luminance = dot(textureColor.rgb, W);
      float thresholdResult = step(threshold, luminance);
      
      gl_FragColor = vec4(vec3(thresholdResult), textureColor.w);
  }
)";

const char * ContrastFragmentShaderString = R"(
#ifdef GL_ES
  precision highp float;
#endif
  varying vec2 textureCoordinate;
  
  uniform sampler2D inputImageTexture;
  uniform float contrast;
  
  void main()
  {
      vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
      
      gl_FragColor = vec4(((textureColor.rgb - vec3(0.5)) * contrast + vec3(0.5)), textureColor.w);
  }
)";

#if TARGET_PLATFORM == PLATFORM_IOS || TARGET_PLATFORM == PLATFORM_ANDROID
const char * BrightnessFragmentShaderString = R"(
 varying highp vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform lowp float brightness;
 
 void main()
 {
     lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     
     gl_FragColor = vec4((textureColor.rgb + vec3(brightness)), textureColor.w);
 }
)";
#else
const char * BrightnessFragmentShaderString = R"(
(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 uniform float brightness;
 
 void main()
 {
     vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     
     gl_FragColor = vec4((textureColor.rgb + vec3(brightness)), textureColor.w);
 }
 )";
#endif

FilterConfig::FilterConfig(const char* name, const char* describe) :
EffectConfig(name, describe, "filter"){
}

FilterConfig* FilterConfig::createByConfig(std::string& filter_config) {
    FilterConfig* newConfig = new FilterConfig("","");
    //TODO: parser json config to init FilterConfig
    
    return newConfig;
}


FilterConfigRef createLuminanceConfig() {
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig("Luminance", ""));
    config->vertex_sh = DefaultFilterVertexShaderString;
    config->fragment_sh = LuminanceFragmentShaderString;
    
    //vertex value
    FilterConfig::VertexConfig positionVC;
    positionVC.key = "a_position";
    positionVC.type = VertexAttrib::VERTEX2;
    positionVC.value.copyForm(Drawable2D::RECTANGLE_COORDS, 8);
    
    FilterConfig::VertexConfig texCoordVC;
    texCoordVC.key = "a_texCoord";
    texCoordVC.type = VertexAttrib::VERTEX2;
    texCoordVC.value.copyForm(Drawable2D::RECTANGLE_TEX_COORDS, 8);
    
    //uniform value
    
    
    config->_vertexConfig.push_back(std::move(positionVC));
    config->_vertexConfig.push_back(std::move(texCoordVC));
    
    config->inputTexture = "inputTexture";
    return config;
}

FilterConfigRef createBrightnessConfig() {
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig("Brightness", "uniform:{@brightness(FLOAT:-1.0 ~ 1.0)}"));
    config->vertex_sh = DefaultFilterVertexShaderString;
    config->fragment_sh = BrightnessFragmentShaderString;
    
    //vertex value
    FilterConfig::VertexConfig positionVC;
    positionVC.key = "a_position";
    positionVC.type = VertexAttrib::VERTEX2;
    positionVC.value.copyForm(Drawable2D::RECTANGLE_COORDS, 8);
    
    FilterConfig::VertexConfig texCoordVC;
    texCoordVC.key = "a_texCoord";
    texCoordVC.type = VertexAttrib::VERTEX2;
    texCoordVC.value.copyForm(Drawable2D::RECTANGLE_TEX_COORDS, 8);
    
    //uniform value
    FilterConfig::UniformConfig brightnessUC;
    brightnessUC.key = "brightness";
    brightnessUC.type = Uniform::FLOAT;
    brightnessUC.value._float_val = 0;
    
    config->_vertexConfig.push_back(std::move(positionVC));
    config->_vertexConfig.push_back(std::move(texCoordVC));
    config->_uniformConfigs.push_back(std::move(brightnessUC));
    
    config->inputTexture = "inputTexture";
    return config;
}

FilterConfigRef createLuminanceThresholdConfig() {
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig("LuminanceThreshold", "uniform:{@threshold(FLOAT)}"));
    config->vertex_sh = DefaultFilterVertexShaderString;
    config->fragment_sh = LuminanceThresholdFragmentShaderString;
    
    //vertex value
    FilterConfig::VertexConfig positionVC;
    positionVC.key = "a_position";
    positionVC.type = VertexAttrib::VERTEX2;
    positionVC.value.copyForm(Drawable2D::RECTANGLE_COORDS, 8);
    
    FilterConfig::VertexConfig texCoordVC;
    texCoordVC.key = "a_texCoord";
    texCoordVC.type = VertexAttrib::VERTEX2;
    texCoordVC.value.copyForm(Drawable2D::RECTANGLE_TEX_COORDS, 8);
    
    //uniform value
    FilterConfig::UniformConfig thresholdUC;
    thresholdUC.key = "threshold";
    thresholdUC.type = Uniform::FLOAT;
    thresholdUC.value._float_val = 0.5f;
    
    config->_vertexConfig.push_back(std::move(positionVC));
    config->_vertexConfig.push_back(std::move(texCoordVC));
    config->_uniformConfigs.push_back(std::move(thresholdUC));
    
    config->inputTexture = "inputTexture";
    return config;
}

FilterConfigRef createContrastConfig() {
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig("Contrast", "uniform:{@contrast(FLOAT)}"));
    config->vertex_sh = DefaultFilterVertexShaderString;
    config->fragment_sh = ContrastFragmentShaderString;
    
    //vertex value
    FilterConfig::VertexConfig positionVC;
    positionVC.key = "a_position";
    positionVC.type = VertexAttrib::VERTEX2;
    positionVC.value.copyForm(Drawable2D::RECTANGLE_COORDS, 8);
    
    FilterConfig::VertexConfig texCoordVC;
    texCoordVC.key = "a_texCoord";
    texCoordVC.type = VertexAttrib::VERTEX2;
    texCoordVC.value.copyForm(Drawable2D::RECTANGLE_TEX_COORDS, 8);
    
    //uniform value
    FilterConfig::UniformConfig contrastUC;
    contrastUC.key = "contrast";
    contrastUC.type = Uniform::FLOAT;
    contrastUC.value._float_val = 1.f;
    
    config->_vertexConfig.push_back(std::move(positionVC));
    config->_vertexConfig.push_back(std::move(texCoordVC));
    config->_uniformConfigs.push_back(std::move(contrastUC));
    
    config->inputTexture = "inputTexture";
    return config;
}

GRAPHICCORE_END
