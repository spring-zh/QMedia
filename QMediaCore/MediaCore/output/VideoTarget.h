//
//  VideoTarget.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "OutputTarget.h"
#include "MediaCore/core/VideoFrame.h"

class VideoRender
{
public:
    //these functions run on render thread
    virtual bool onRenderCreate() = 0;
    virtual bool onVideoRender(int64_t wantTimeMs) = 0;
    virtual bool onRenderDestroy() = 0;
    
    //set output display mode
//    virtual void setDisplayMode(DisplayMode mode, int dstW, int dstH) = 0;
};

class VideoTarget :public OutputTarget{
public:
    
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
