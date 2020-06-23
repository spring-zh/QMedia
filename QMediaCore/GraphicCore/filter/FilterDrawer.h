//
//  FilterDrawer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_TEXTUREFILTER_H
#define GRAPHICCORE_TEXTUREFILTER_H

#include <memory>
#include <map>
#include "Utils/Comm.h"
#include "FilterConfig.h"
#include "GraphicCore/opengl/Texture2D.h"

GRAPHICCORE_BEGIN

/** FilterDrawer must be call on gl thread
 *  use texture path "inputTexture" to set default input texture uniform
 */
class FilterDrawer {
public:
    explicit FilterDrawer(FilterConfig * config);
    virtual ~FilterDrawer();
    
    virtual void drawFilter(int64_t duration, const Texture2D *inputTexture, std::map<std::string, Uniform::Value>& parameters, const Mat4 & transform);

protected:
    ShaderProgram _shaderProgram;
    std::string _inputTexture; //key of inputTexture
};

using FilterDrawerRef = std::shared_ptr<FilterDrawer>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_TEXTUREFILTER_H */
