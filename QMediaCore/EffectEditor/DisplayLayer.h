//
//  DisplayLayer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_DIAPLAY_LAYER_H
#define EFFECTEDITOR_DIAPLAY_LAYER_H

#include "RenderEngine/Layer2.h"

namespace QMedia { namespace Api {

enum class DisplayMode : int{
    Stretch = 0,
    Adaptive,
    Clip
};

class MediaSessionImpl;

class DisplayLayer : public Layer2 {
public:
    DisplayLayer(MediaSessionImpl *session);
    ~DisplayLayer(){}
    
    const Range<int64_t> getRange() override;
    
    void releaseRes() override;
    
    bool beginRender() ;
    void setLayerSize(const RenderEngine::Size& size) override ;
    void draw(Api::Scene* scene, const Api::Mat4 & transform, uint32_t flags) override ;
    
    void setTargetSize(int viewW, int viewH) ;
    void setDisplayMode(DisplayMode mode, bool filp_v) ;
    void render(int64_t timeStamp, bool no_display) ;

    void readRGB(void* out_data, int width, int height, int32_t format) ;
    
    bool _isInit;

private:
    MediaSessionImpl * session_;
    RenderEngine::Scene _playerScene;
    RenderEngine::RenderDeviceGL::UPtr _gle;
    
    DisplayMode _displayMode;
    bool filp_v_ = false;
    int _viewW,_viewH;
    
    GLuint out_pbo_ = 0;
    bool no_display_ = false;
    float gl_version_ = 2.0f;
};

}
}

#endif /* EFFECTEDITOR_DIAPLAY_LAYER_H */
