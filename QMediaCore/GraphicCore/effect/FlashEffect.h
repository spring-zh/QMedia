//
//  FlashEffect.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_FLASHEFFECT_H
#define GRAPHICCORE_FLASHEFFECT_H

#include "Effect.h"
#include "GraphicCore/filter/Filter.h"

GRAPHICCORE_BEGIN

class FlashEffect : public  Effect{
public:
    FlashEffect() { _duration = 60; };
    ~FlashEffect() {};
    
    bool load() override {
        _brightness_drawer = FilterFactory::instance()->getFilterDrawer("Brightness");
        return true;
    }
    bool unload() override {
        return true;
    }
    void drawEffect(int64_t duration, const Texture2D *inputTexture, GraphicCore::GLEngine* gle) override {
        int n = duration / _duration;
        float t = (float)duration/_duration - n;
        float val = (n & 1)?(1.f - t*2):(t*2 - 1.f);
        Mat4 transform;
        std::map<std::string, Uniform::Value> parameters;
        Uniform::Value uniform_val;
        uniform_val._float_val = val;
        parameters["brightness"] = uniform_val;
        _brightness_drawer->drawFilter(duration, inputTexture, parameters, transform);
    }

    void setIntValue(const char* key, int value) override {
        _duration = value;
    }
    void setIntArrayValue(const char* key, int value[], int count) override {
        
    }
    void setFloatValue(const char* key, float value) override {
        
    }
    void setFloatArrayValue(const char* key, float value[], int count) override {
        
    }
    
    static EffectConfig* createConfig() {
        static const char * describe = R"({ "duration": {"type":"INT", "default": 60, "max": 200 , "min": 10} })";
        EffectConfig* effectConfig = new EffectConfig("FlashEffect",describe,"effect");
        effectConfig->effect_create = []()->Effect*{ return new FlashEffect(); };
        return effectConfig;
    }
    
private:
    FilterDrawer* _brightness_drawer;
    int _duration;
};

GRAPHICCORE_END

#endif /* GRAPHICCORE_FLASHEFFECT_H */
