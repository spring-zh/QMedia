//
//  EffectManage.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_EFFECTMANAGE_H
#define GRAPHICCORE_EFFECTMANAGE_H

#include "Effect.h"
#include "GraphicCore/filter/Filter.h"

GRAPHICCORE_BEGIN

using EffectConfigRef = std::shared_ptr<EffectConfig>;

class EffectManage {
public:
    ~EffectManage();
    
    static EffectManage* instance();
    
    //add filter config by json
    void addFilterConfig(std::string& config_json);
    //add effect config by json
    void addEffectConfig(std::string& config_json);

    std::vector<EffectConfig*> getAllEffectConfig();
    Effect* createEffect(const char* effect_name);
    
private:
    void addEffectConfig(EffectConfig* config);
    EffectManage();
    std::mutex _config_mutex;
    std::map<std::string , EffectConfigRef> _effectConfigs;
};

GRAPHICCORE_END

#endif /* GRAPHICCORE_EFFECT_H */
