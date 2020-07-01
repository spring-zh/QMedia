//
//  VideoFrameDrawer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_VIDEOFRAME_DRAWER_H
#define EFFECTEDITOR_VIDEOFRAME_DRAWER_H

#include "Utils/Comm.h"
#include "GraphicCore/opengl/Texture2D.h"
#include "GraphicCore/GcNode.h"
#include "MediaCore/mediadef/VideoFrame.h"
#include "MediaCore/output/VideoTarget.h"

class VideoFrameDrawer{
public:
    VideoFrameDrawer(const VideoTarget *videoTarget):_videoTarget(videoTarget) {}
    virtual ~VideoFrameDrawer(){}

    virtual bool setFrame(const VideoFrame& videoFrame) = 0;
    virtual void drawFrame(const GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, const GraphicCore::Node* node) = 0;
    virtual void drawFrameDirect(const GraphicCore::Scene* /*scene*/, const GraphicCore::Rect & /*region*/, const GraphicCore::Rect crop, GraphicCore::Color4F color, GraphicCore::Drawable2D::FlipMode flipMode) = 0;
    virtual const GraphicCore::Texture2D* getOutputTexture() { return nullptr; }
    virtual void release() = 0;
    
protected:
    const VideoTarget *_videoTarget;
};

#endif /* EFFECTEDITOR_VIDEOFRAME_DRAWER_H */
