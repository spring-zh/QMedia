//
//  VideoTarget.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "OutputTarget.h"
#include "MediaCore/mediadef/VideoFrame.h"

class VideoRender
{
public:
    //these functions run on render thread
    virtual bool onRenderCreate() = 0;
    virtual bool onVideoRender(int64_t wantTimeMs) = 0;
    virtual bool onRenderDestroy() = 0;
//    virtual bool onVideoRenderWithTextureId(int textureId) = 0;
};

class VideoTarget :public OutputTarget{
public:
    
//    typedef struct Resolution {
//        int width;
//        int height;
//    }Resolution;
    
	virtual ~VideoTarget(){}

    virtual bool  init(VideoDescribe videoDesc) = 0;
	virtual void * getRenderContext() const { return NULL; } //
    virtual const int getWidth() const = 0;
    virtual const int getHeight() const = 0;
    virtual const float getFrameRate() const = 0;
    
    virtual void setRender(VideoRender * render) { _render = render; }
protected:
	VideoRender * _render;
	
};
