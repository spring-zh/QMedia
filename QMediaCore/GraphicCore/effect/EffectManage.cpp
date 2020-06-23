//
//  EffectManage.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectManage.h"

GRAPHICCORE_BEGIN

EffectManage::~EffectManage() {
    
}

EffectManage* EffectManage::instance() {
    static EffectManage effect_manage;
    return &effect_manage;
}

void EffectManage::addFilterConfig(std::string& config_json) {
    FilterFactory::instance()->addNewConfig(config_json);
}

void EffectManage::addEffectConfig(std::string& config_json) {
    
}

std::vector<EffectConfig*> EffectManage::getAllEffectConfig() {
    std::vector<FilterConfig*> filterconfigs = FilterFactory::instance()->getAllFilterConfig();
    std::vector<EffectConfig*> effectlists;
    for (auto& iter : _effectConfigs) {
        effectlists.push_back(iter.second.get());
    }
    for (auto& filtercondig : filterconfigs) {
        effectlists.push_back(filtercondig);
    }
    return effectlists;
}
Effect* EffectManage::createEffect(const char* effect_name) {
    Effect* effect = FilterFactory::instance()->createFilter(effect_name);
    if (effect == nullptr) {
        //TODO: not filter, create other effect
    }
    return effect;
}

GRAPHICCORE_END
