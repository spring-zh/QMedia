//
//  FilterConfig.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_FILTERCONFIG_H
#define GRAPHICCORE_FILTERCONFIG_H

#include <vector>
#include <string>
#include "RenderEngine/effect/Effect.h"
#include "RenderEngine/opengl/shader_program_gl.h"

namespace QMedia {
namespace RenderEngine {

class FilterConfig : public EffectConfig {
public:
    class UniformConfig {
    public:
        std::string key;
        Uniform::Type type;
        Uniform::Value value;
    };
    
    class VertexConfig {
    public:
        std::string key;
        VertexAttrib::Type type;
        VertexAttrib::Value value;
    };
    
    FilterConfig(const char* name, const char* describe);
    static FilterConfig* createByConfig(std::string& filter_config);
    
    ~FilterConfig() = default;

    std::vector<UniformConfig> _uniformConfigs;
    std::vector<VertexConfig> _vertexConfig;
    
    std::string vertex_sh;
    std::string fragment_sh;
    
    std::string inputTexture;

};

using FilterConfigRef = std::shared_ptr<FilterConfig>;

//internal filter define
extern const char* DefaultFilterVertexShaderString;

FilterConfigRef createLuminanceConfig();
FilterConfigRef createBrightnessConfig();
FilterConfigRef createRGBAConfig();
FilterConfigRef createLuminanceThresholdConfig();
FilterConfigRef createContrastConfig();
FilterConfigRef createColorInvertConfig();
FilterConfigRef createPolarPixellateConfig();
FilterConfigRef createToonConfig();
FilterConfigRef createWhiteBalanceConfig();
FilterConfigRef createSobelEdgeDetectionConfig();

}
}

#endif /* GRAPHICCORE_FILTERCONFIG_H */
