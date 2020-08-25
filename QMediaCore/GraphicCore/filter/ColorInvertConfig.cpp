//
//  PolarPixellateConfig.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FilterConfig.h"
#include "GraphicCore/opengl/Drawable2D.h"

#define FILTERNAME "ColorInvert"

GRAPHICCORE_BEGIN

#if TARGET_PLATFORM == PLATFORM_IOS || TARGET_PLATFORM == PLATFORM_ANDROID
const char * ColorInvertFragmentShaderString = R"(
  varying highp vec2 textureCoordinate;
  
  uniform sampler2D inputImageTexture;
  
  void main()
  {
     lowp vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     
     gl_FragColor = vec4((1.0 - textureColor.rgb), textureColor.w);
  }
)";
#else
const char * ColorInvertFragmentShaderString = R"(
 varying vec2 textureCoordinate;
 
 uniform sampler2D inputImageTexture;
 
 void main()
 {
     vec4 textureColor = texture2D(inputImageTexture, textureCoordinate);
     
     gl_FragColor = vec4((1.0 - textureColor.rgb), textureColor.w);
 }
)";
#endif

const char * ColorInvertDescribe = "";

FilterConfigRef createColorInvertConfig() {
    
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig(FILTERNAME, ColorInvertDescribe));
    config->vertex_sh = DefaultFilterVertexShaderString;
    config->fragment_sh = ColorInvertFragmentShaderString;
    
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

GRAPHICCORE_END
