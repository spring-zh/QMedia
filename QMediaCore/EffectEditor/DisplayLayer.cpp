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
    if (out_pbo_) {
        glDeleteBuffers(1, &out_pbo_);
    }
    Layer2::releaseRes();
    _gle->releaseAll();
}

bool DisplayLayer::beginRender() {
    LOGI("beginRender");
    if (!_isInit) {
        _gle->useAsDefaultFrameBuffer();
        createRes();
        _isInit = true;
    }
    gl_version_ = RenderDeviceGL::GetVersion();
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

void DisplayLayer::render(int64_t timeStamp, bool no_display){
    no_display_ = no_display;
    _playerScene.setDelta(timeStamp);
    // read dec frame to medianode
    session_->ReadVideoFrames(timeStamp);
    RenderEngine::Rect region = calculateDisplayRegion(_displayMode, getLayerSize().width, getLayerSize().height, _viewW, _viewH);

    setPosition(Point(region.origin.x, region.origin.y));
    setContentSize(Size(region.size.width, region.size.height));
    _gle->setViewPort(Api::Vec4f(0, 0, _viewW, _viewH));
    Layer2::visit(&_playerScene, _playerScene.getMatrix(MATRIX_STACK_MODELVIEW), 0);
}

void DisplayLayer::draw(Api::Scene* scene, const Api::Mat4 & transform, uint32_t flags) {
    if (!no_display_) {
        Layer2::draw(scene, transform, flags);
    }
}

void DisplayLayer::readRGB(void* out_data, int width, int height, int32_t format) {
    Holder<TextureFrameBuffer> holder(fbo_texture_.get());
    int dataSize = width * height * 4;
#if GL_ES_VERSION_3_0
    if (gl_version_ >= 3.0) {
        if (!out_pbo_) {
            glGenBuffers(1, &out_pbo_);
            CHECK_GL_ERROR(glBindBuffer(GL_PIXEL_PACK_BUFFER, out_pbo_));
            CHECK_GL_ERROR(glBufferData(GL_PIXEL_PACK_BUFFER, dataSize, 0, GL_STREAM_DRAW));
        } else {
            CHECK_GL_ERROR(glBindBuffer(GL_PIXEL_PACK_BUFFER, out_pbo_));
        }
        
        CHECK_GL_ERROR(glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, nullptr));
        GLvoid *bufPtr = glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, dataSize, GL_MAP_READ_BIT);
        if (bufPtr) {
            memcpy(out_data, bufPtr, dataSize);
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        }
        CHECK_GL_ERROR(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
    }
#endif
    else {
        CHECK_GL_ERROR(glReadPixels(0, 0, width, height, format, GL_UNSIGNED_BYTE, out_data));
    }
}
    
}
}
