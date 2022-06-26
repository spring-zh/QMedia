//
//  DisplayLayer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Utils/Logger.h"
#include "DisplayLayer.h"
#include "MediaSessionImpl.h"

namespace QMedia { namespace Api {

using namespace RenderEngine;

RenderEngine::Rect calculateDisplayRegion(DisplayMode mode, int srcW, int srcH, int dstW, int dstH) {
    RenderEngine::Rect dstRegion;
    switch (mode) {
        case DisplayMode::Stretch:
            dstRegion.size.width = dstW;
            dstRegion.size.height = dstH;
            break;
        case DisplayMode::Adaptive:
        {
            float dstRatio = (float) dstW / dstH;
            float srcRatio = (float) srcW / srcH;
            if (srcRatio > dstRatio) {
                dstRegion.origin.x = 0;
                dstRegion.size.width = dstW;
                int newH = (int) (dstW / srcRatio);
                dstRegion.origin.y = (dstH - newH)/2;
                dstRegion.size.height = newH;
            } else {
                dstRegion.origin.y = 0;
                dstRegion.size.height = dstH;
                int newW = (int) (dstH * srcRatio);
                dstRegion.origin.x = (dstW - newW) / 2;
                dstRegion.size.width = newW;
            }
        }
            break;
        case DisplayMode::Clip:

            break;

    }
    return dstRegion;
}

DisplayLayer::DisplayLayer(MediaSessionImpl *session): Layer2(RenderEngine::Size(0,0)),
session_(session),
_displayMode(DisplayMode::Adaptive) {
    _gle = RenderDeviceGL::createDevice(0);
    _playerScene.setGLEngine(_gle.get());
    _renderRange = Range<int64_t>(0, std::numeric_limits<int64_t>::max());
    _isInit = false;
}

    
const Range<int64_t> DisplayLayer::getRange() {
    auto range = session_->getTotalTimeRange();
    return Range<int64_t>(range.start, range.end);
}

void DisplayLayer::releaseRes() {
    _gle->releaseAll();
    SceneNode::releaseRes();
}

bool DisplayLayer::beginRender() {
    LOGI("beginRender");
    if (!_isInit) {
        _gle->useAsDefaultFrameBuffer();
//            _targetW = getContentSize().width;
//            _targetH = getContentSize().height;
//            _layerSize = GraphicCore::Size(_targetW, _targetH);
//            _layerSize = getContentSize();
        createRes();
        _isInit = true;
    }

    return true;
}
void DisplayLayer::setLayerSize(const RenderEngine::Size& size) {
    LOGI("setLayerSize width:%f, height:%f", size.width, size.height);
    Layer2::setLayerSize(size);
}
void DisplayLayer::setTargetSize(int viewW, int viewH) {
    LOGI("setTargetSize viewW:%d, viewH:%d", viewW, viewH);
    _viewW = viewW;
    _viewH = viewH;
    RenderEngine::Rect vp(0, 0, _viewW, _viewH);
    _playerScene.setViewPort(vp);
    _playerScene.setProjection(PROJ_2D);
}
void DisplayLayer::setDisplayMode(DisplayMode mode, bool filp_v) {
    LOGI("setDisplayMode");
    _displayMode = mode;
    if (filp_v) {
        _flipMode = Drawable2D::FlipMode::FlipV;
    } else {
        _flipMode = Drawable2D::FlipMode::NONE;
    }
    filp_v_ = filp_v;
}

void DisplayLayer::render(int64_t timeStamp){
    _playerScene.setDelta(timeStamp);
    // read dec frame to medianode
    session_->ReadVideoFrames(timeStamp);
    RenderEngine::Rect region = calculateDisplayRegion(_displayMode, getLayerSize().width, getLayerSize().height, _viewW, _viewH);
//        setPosition(region.origin);
//        setContentSize(region.size);
    setPosition(Point(region.origin.x, region.origin.y));
    setContentSize(Size(region.size.width, region.size.height));
    _gle->setViewPort(Api::Vec4f(0, 0, _viewW, _viewH));
    Layer2::visit(&_playerScene, _playerScene.getMatrix(MATRIX_STACK_MODELVIEW), 0);
}
    
}
}
