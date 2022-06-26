//
//  DisplayLayer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_DIAPLAY_LAYER_H
#define EFFECTEDITOR_DIAPLAY_LAYER_H

#include "GraphicCore/Layer2.h"

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
    void setLayerSize(const GraphicCore::Size& size) override ;
    void setTargetSize(int viewW, int viewH) ;
    void setDisplayMode(DisplayMode mode, bool filp_v) ;
    void render(int64_t timeStamp) ;
    
    bool _isInit;

private:
    MediaSessionImpl * session_;
    GraphicCore::Scene _playerScene;
    GraphicCore::GLEngine _gle;
    
    DisplayMode _displayMode;
    bool filp_v_ = false;
    int _viewW,_viewH;
};

}
}

#endif /* EFFECTEDITOR_DIAPLAY_LAYER_H */
