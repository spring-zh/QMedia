//
//  FilterFactory.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_FILTERFACTORY_H
#define GRAPHICCORE_FILTERFACTORY_H

#include "FilterDrawer.h"

GRAPHICCORE_BEGIN;

class FilterFactory;

class Filter : public Effect{
public:
    
    void setIntValue(const char* key, int value) override;
    void setIntArrayValue(const char* key, int value[], int count) override;
    
    void setFloatValue(const char* key, float value) override;
    void setFloatArrayValue(const char* key, float value[], int count) override;
    
    virtual bool load() override { return true; }
    virtual bool unload() override { return true; }
    virtual void drawEffect(int64_t duration, const Texture2D *inputTexture, GraphicCore::GLEngine* gle) override;

private:
    void setParameter(const char* key, Uniform::Value& value);
    
    friend class FilterFactory;
    explicit Filter(FilterFactory* factory):_factory(factory),_filter_drawer(nullptr) {
    }
    FilterFactory* _factory;
    FilterDrawer* _filter_drawer;
    
    std::map<std::string, Uniform::Value> _parameters;
};

class FilterFactory {
public:
    
    ~FilterFactory();
    
    static FilterFactory* instance();
    
    //add new filter config by json
    void addNewConfig(std::string& config_json);
    //add new filter config
    void addNewConfig(FilterConfigRef config);
    
    std::vector<FilterConfig*> getAllFilterConfig();
    
    
    Filter* createFilter(const char* filter_name);
    //release all gl filter resource
    void clearTextureFilters();
    FilterDrawer* getFilterDrawer(const char* filter_name);

private:
    FilterFactory();
    friend class Filter;
    FilterDrawer* getFilterDrawer(FilterConfig* config);
    
    std::map<std::string , FilterDrawerRef> _filterDrawers;
    
    std::mutex _config_mutex;
    std::map<std::string , FilterConfigRef> _filterConfigs;
};

using FilterRef = std::shared_ptr<Filter>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_FILTERFACTORY_H */
