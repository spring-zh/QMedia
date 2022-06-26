//
//  PolarPixellateConfig.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FilterConfig.h"
#include "RenderEngine/opengl/Drawable2D.h"

#define FILTERNAME "PolarPixellate"

namespace QMedia {
namespace RenderEngine {

const char * PolarPixellateFragmentShaderString = R"(
#ifdef GL_ES
 precision highp float;
#endif
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
)";

const char * PolarPixellateDescribe = R"({ "pixelSize": {"type":"FLOAT2", "default": [0.05, 0.05], "max": [0.1, 0.1] , "min": [-0.1, -0.1]} })";

FilterConfigRef createPolarPixellateConfig() {
    
    std::shared_ptr<FilterConfig> config = std::shared_ptr<FilterConfig>(new FilterConfig(FILTERNAME, PolarPixellateDescribe));
    config->vertex_sh = DefaultFilterVertexShaderString;
    config->fragment_sh = PolarPixellateFragmentShaderString;
    
    //vertex value
    FilterConfig::VertexConfig positionVC;
    positionVC.key = "a_position";
    positionVC.type = VertexAttrib::VERTEX2;
    positionVC.value = Variant(Drawable2D::RECTANGLE_COORDS, 8);
    
    FilterConfig::VertexConfig texCoordVC;
    texCoordVC.key = "a_texCoord";
    texCoordVC.type = VertexAttrib::VERTEX2;
    texCoordVC.value = Variant(Drawable2D::RECTANGLE_TEX_COORDS, 8);
    
    FilterConfig::UniformConfig centerUC;
    centerUC.key = "center";
    centerUC.type = Uniform::FLOAT2;
    centerUC.value = std::vector<float>{0.5f, 0.5f};
    
    FilterConfig::UniformConfig pixelSizeUC;
    pixelSizeUC.key = "pixelSize";
    pixelSizeUC.type = Uniform::FLOAT2;
    pixelSizeUC.value = std::vector<float>{0.05f, 0.05f};
    
    config->_vertexConfig.push_back(std::move(positionVC));
    config->_vertexConfig.push_back(std::move(texCoordVC));
    config->_uniformConfigs.push_back(std::move(centerUC));
    config->_uniformConfigs.push_back(std::move(pixelSizeUC));
    
    config->inputTexture = "inputTexture";
    return config;
}

}
}
