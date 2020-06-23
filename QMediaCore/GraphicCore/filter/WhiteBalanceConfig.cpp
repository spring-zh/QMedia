//
//  WhiteBalanceConfig.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FilterConfig.h"
#include "GraphicCore/opengl/Drawable2D.h"

#define FILTERNAME "WhiteBalance"

GRAPHICCORE_BEGIN

#if TARGET_PLATFORM == PLATFORM_IOS || TARGET_PLATFORM == PLATFORM_ANDROID
const char * WhiteBalanceFragmentShaderString = R"(
  uniform sampler2D inputImageTexture;
  varying highp vec2 textureCoordinate;
   
  uniform lowp float temperature;
  uniform lowp float tint;

  const lowp vec3 warmFilter = vec3(0.93, 0.54, 0.0);

  const mediump mat3 RGBtoYIQ = mat3(0.299, 0.587, 0.114, 0.596, -0.274, -0.322, 0.212, -0.523, 0.311);
  const mediump mat3 YIQtoRGB = mat3(1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.105, 1.702);

  void main()
  {
      lowp vec4 source = texture2D(inputImageTexture, textureCoordinate);
      
      mediump vec3 yiq = RGBtoYIQ * source.rgb; //adjusting tint
      yiq.b = clamp(yiq.b + tint*0.5226*0.1, -0.5226, 0.5226);
      lowp vec3 rgb = YIQtoRGB * yiq;

      lowp vec3 processed = vec3(
          (rgb.r < 0.5 ? (2.0 * rgb.r * warmFilter.r) : (1.0 - 2.0 * (1.0 - rgb.r) * (1.0 - warmFilter.r))), //adjusting temperature
          (rgb.g < 0.5 ? (2.0 * rgb.g * warmFilter.g) : (1.0 - 2.0 * (1.0 - rgb.g) * (1.0 - warmFilter.g))),
          (rgb.b < 0.5 ? (2.0 * rgb.b * warmFilter.b) : (1.0 - 2.0 * (1.0 - rgb.b) * (1.0 - warmFilter.b))));

      gl_FragColor = vec4(mix(rgb, processed, temperature), source.a);
  }
)";
#else
const char * WhiteBalanceFragmentShaderString = R"(
  uniform sampler2D inputImageTexture;
  varying vec2 textureCoordinate;
  
  uniform float temperature;
  uniform float tint;
  
  const vec3 warmFilter = vec3(0.93, 0.54, 0.0);
  
  const mat3 RGBtoYIQ = mat3(0.299, 0.587, 0.114, 0.596, -0.274, -0.322, 0.212, -0.523, 0.311);
  const mat3 YIQtoRGB = mat3(1.0, 0.956, 0.621, 1.0, -0.272, -0.647, 1.0, -1.105, 1.702);
  
  void main()
 {
     vec4 source = texture2D(inputImageTexture, textureCoordinate);
     
     vec3 yiq = RGBtoYIQ * source.rgb; //adjusting tint
     yiq.b = clamp(yiq.b + tint*0.5226*0.1, -0.5226, 0.5226);
     vec3 rgb = YIQtoRGB * yiq;
     
     vec3 processed = vec3(
                                (rgb.r < 0.5 ? (2.0 * rgb.r * warmFilter.r) : (1.0 - 2.0 * (1.0 - rgb.r) * (1.0 - warmFilter.r))), //adjusting temperature
                                (rgb.g < 0.5 ? (2.0 * rgb.g * warmFilter.g) : (1.0 - 2.0 * (1.0 - rgb.g) * (1.0 - warmFilter.g))),
                                (rgb.b < 0.5 ? (2.0 * rgb.b * warmFilter.b) : (1.0 - 2.0 * (1.0 - rgb.b) * (1.0 - warmFilter.b))));
     
     gl_FragColor = vec4(mix(rgb, processed, temperature), source.a);
 }
)";
#endif

const char * WhiteBalanceDescribe = "uniform:{@temperature(FLOAT),@tint(FLOAT)}";

FilterConfigRef createWhiteBalanceConfig() {
    
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig(FILTERNAME, WhiteBalanceDescribe));
    config->vertex_sh = DefaultFilterVertexShaderString;
    config->fragment_sh = WhiteBalanceFragmentShaderString;
    
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
    
    //_temperature < 5000 ? 0.0004 * (_temperature-5000.0) : 0.00006 * (_temperature-5000.0)
    FilterConfig::UniformConfig temperatureUC;
    temperatureUC.key = "temperature";
    temperatureUC.type = Uniform::FLOAT;
    temperatureUC.value._float_val = 0;
    
    FilterConfig::UniformConfig tintUC;
    tintUC.key = "tint";
    tintUC.type = Uniform::FLOAT;
    tintUC.value._float_val = 0;
    
    config->_vertexConfig.push_back(std::move(positionVC));
    config->_vertexConfig.push_back(std::move(texCoordVC));
    config->_uniformConfigs.push_back(std::move(temperatureUC));
    config->_uniformConfigs.push_back(std::move(tintUC));
    
    config->inputTexture = "inputTexture";
    return config;
}

GRAPHICCORE_END
