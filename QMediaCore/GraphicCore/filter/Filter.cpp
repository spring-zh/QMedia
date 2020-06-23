//
//  FilterFactory.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Filter.h"

GRAPHICCORE_BEGIN

#pragma mark FilterObject
void Filter::setIntValue(const char* key, int value) {
    Uniform::Value uniform_val;
    uniform_val._int_val = value;
    setParameter(key, uniform_val);
}
void Filter::setIntArrayValue(const char* key, int value[], int count) {
    Uniform::Value uniform_val;
    uniform_val.copyForm(value, count);
    setParameter(key, uniform_val);
}

void Filter::setFloatValue(const char* key, float value) {
    Uniform::Value uniform_val;
    uniform_val._float_val = value;
    setParameter(key, uniform_val);
}
void Filter::setFloatArrayValue(const char* key, float value[], int count) {
    Uniform::Value uniform_val;
    uniform_val.copyForm(value, count);
    setParameter(key, uniform_val);
}
void Filter::setParameter(const char* key, Uniform::Value& value) {
    _parameters[key] = std::move(value);
}

void Filter::drawEffect(int64_t duration, const Texture2D *inputTexture, GraphicCore::GLEngine* gle) {
    if (!_filter_drawer) {
        _filter_drawer = _factory->getFilterDrawer(dynamic_cast<FilterConfig*>(_config));
        if (!_filter_drawer) return;
    }
    Mat4 transform;
    _filter_drawer->drawFilter(duration, inputTexture, _parameters, transform);
}

#pragma mark FilterFactory

//std::map<std::string , FilterConfigRef> FilterFactory::g_filterConfigs;

FilterFactory::FilterFactory() {
    addNewConfig(createColorInvertConfig());
    addNewConfig(createPolarPixellateConfig());
    addNewConfig(createToonConfig());
    addNewConfig(createLuminanceConfig());
    addNewConfig(createBrightnessConfig());
    addNewConfig(createSobelEdgeDetectionConfig());
    addNewConfig(createWhiteBalanceConfig());
    addNewConfig(createLuminanceThresholdConfig());
    addNewConfig(createContrastConfig());
}
FilterFactory::~FilterFactory() {
    
}

FilterFactory* FilterFactory::instance() {
    static FilterFactory factory;
    return &factory;
}

void FilterFactory::addNewConfig(std::string& config_json) {
    FilterConfig* config = FilterConfig::createByConfig(config_json);
    if (config) {
        addNewConfig(FilterConfigRef(config));
    }
}
void FilterFactory::addNewConfig(FilterConfigRef config) {
    std::unique_lock<std::mutex> config_lck(_config_mutex);
    if (_filterConfigs.find(config->name) == _filterConfigs.end()) {
        _filterConfigs[config->name] = config;
    }
}

void FilterFactory::clearTextureFilters() {
    _filterDrawers.clear();
}

Filter* FilterFactory::createFilter(const char* filter_name) {
    auto iter = _filterConfigs.find(filter_name);
    if (iter != _filterConfigs.end()) {
        Filter* filter = new Filter(this);
        filter->_config = iter->second.get();
        return filter;
    }
    return nullptr;
}

std::vector<FilterConfig*> FilterFactory::getAllFilterConfig() {
    std::unique_lock<std::mutex> config_lck(_config_mutex);
    std::vector<FilterConfig*> filterlists;
    for (auto& iter : _filterConfigs) {
        filterlists.push_back(iter.second.get());
    }
    return filterlists;
}

FilterDrawer* FilterFactory::getFilterDrawer(FilterConfig* config) {
    auto iter = _filterDrawers.find(config->name);
    FilterDrawer* retDrawer;
    if (iter == _filterDrawers.end()) {
        retDrawer = new FilterDrawer(config);
        _filterDrawers[config->name] = FilterDrawerRef(retDrawer);
    }else {
        retDrawer = iter->second.get();
    }
    return retDrawer;
}

GRAPHICCORE_END
