//
//  FilterDrawer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FilterDrawer.h"
#include "GraphicCore/base/Image.h"

GRAPHICCORE_BEGIN

FilterDrawer::FilterDrawer(FilterConfig * config) {
    _inputTexture = config->inputTexture;
    if (_shaderProgram.createProgram(config->vertex_sh.c_str(), config->fragment_sh.c_str())) {
        // init uniform
        for (auto& uniformConfig : config->_uniformConfigs) {
            _shaderProgram.setUniformValue(uniformConfig.key.c_str(), uniformConfig.type, uniformConfig.value);
        }
        // init attribe
        for (auto& attribeConfig : config->_vertexConfig) {
            _shaderProgram.setVertexAttribValue(attribeConfig.key.c_str(), attribeConfig.type, attribeConfig.value);
        }
    }
}
FilterDrawer::~FilterDrawer() {
    _shaderProgram.releaseProgram();
}

void FilterDrawer::drawFilter(int64_t duration, const Texture2D *inputTexture, std::map<std::string, Uniform::Value>& parameters, const Mat4 & transform) {
    if (_shaderProgram.use()) {
        //set default input texture
        Uniform::Value uniform_val;
        uniform_val._texture = inputTexture->getTextureId();
        uniform_val._textureTarget = inputTexture->getTextureTarget();
        _shaderProgram.setUniformValue(_inputTexture.c_str(), Uniform::TEXTURE, uniform_val);
        
        //update extern parameters
        for (auto &iter : parameters) {
            _shaderProgram.updateUniformValue(iter.first.c_str(), iter.second);
        }
        
        _shaderProgram.drawRectangle();
    }
}

GRAPHICCORE_END
