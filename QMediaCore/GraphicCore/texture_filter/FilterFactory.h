//
//  FilterFactory.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_FILTERFACTORY_H
#define GRAPHICCORE_FILTERFACTORY_H

#include <memory>
#include <map>
#include "Utils/Comm.h"
#include "GraphicCore/base/Macros.h"
#include "GraphicCore/opengl/ShaderProgram.h"
#include "GraphicCore/opengl/Texture2D.h"

GRAPHICCORE_BEGIN

class TextureFilter {
public:
    virtual ~TextureFilter() {}
    
    // call on gl thread
    virtual bool create() = 0;
    virtual void drawFilter(int64_t duration, const Texture2D *srcTexture, int flags) = 0;
    virtual void release() = 0;
    
    void setParameter(std::string key, Uniform::Value& value) {
        _parameters[key] = value;
    }
#if 0
    //set value in key
    template <typename T>
    void setValue(std::string key, const T& value) {
        _parameters[key] = ValueToString<decltype(value)>(value);
    }
    /**
     * get value by key,  return default_value when key not set
     */
    template <typename T>
    T getValue(std::string key, T default_value) {
        T ret_value = default_value;
        auto iter = _parameters.find(key);
        if (iter != _parameters.end()) {
            ret_value = StringToValue<T>(iter->second);
        }
        return ret_value;
    }
#endif
protected:
    std::map<std::string, Uniform::Value> _parameters;
    ShaderProgram _shaderProgram;
};

enum class FilterType : int {
    POLARPIXELLATE,
    Default,
};

class TextureFilterFactory {
public:
    TextureFilterFactory() = default;
    ~TextureFilterFactory() = default;
    
    static TextureFilter* createFilter(FilterType filterType);
};

using TextureFilterRef = std::shared_ptr<TextureFilter>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_FILTERFACTORY_H */
