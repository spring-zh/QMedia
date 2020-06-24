//
//  Effect.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_EFFECT_H
#define GRAPHICCORE_EFFECT_H

#include <memory>
#include <map>
#include "Utils/Comm.h"
#include "GraphicCore/GcScene.h"

GRAPHICCORE_BEGIN

class Effect;
typedef Effect* (*effectCreate)();

class EffectConfig {
public:
    EffectConfig(const char* name, const char* describe, const char* type) {
        this->name = name;
        this->describe = describe;
        this->effect_type = type;
    }
    virtual ~EffectConfig() = default;
    std::string name; //effect name
    std::string describe; //effect describe
    std::string effect_type; //effect type
    
    effectCreate effect_create = nullptr; //effect create function
};

class Effect {
public:
    virtual ~Effect() {};
    
    // load/unload gl resource, must call in gl thread
    virtual bool load() = 0;
    virtual bool unload() = 0;
    virtual void drawEffect(int64_t duration, const Texture2D *inputTexture, GraphicCore::GLEngine* gle) = 0;
    
    //set extern value
    virtual void setIntValue(const char* key, int value) = 0;
    virtual void setIntArrayValue(const char* key, int value[], int count) = 0;
    virtual void setFloatValue(const char* key, float value) = 0;
    virtual void setFloatArrayValue(const char* key, float value[], int count) = 0;
    
    const EffectConfig* getConfig() const { return _config; }
    Range<int64_t> _timeRange; //display time range
protected:
    EffectConfig* _config;
};

using EffectRef = std::shared_ptr<Effect>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_EFFECT_H */
