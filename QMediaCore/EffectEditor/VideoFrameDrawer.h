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
#include "GraphicCore/SceneNode.h"
#include "GraphicCore/GcScene.h"
#include "MediaCore/core/VideoFrame.h"
#include "MediaCore/output/VideoTarget.h"

class VideoFrameDrawer{
public:
    static VideoFrameDrawer* createVideoFrameDrawer(RawVideoFormat pixel_format);
    
    VideoFrameDrawer():_videoTarget(nullptr) {}
    VideoFrameDrawer(const VideoTarget *videoTarget):_videoTarget(videoTarget) {}
    virtual ~VideoFrameDrawer(){}

    virtual bool setFrame(const VideoFrame& videoFrame) = 0;
    virtual void drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, QMedia::Api::SceneNode* node) = 0;
    virtual void drawFrame(const GraphicCore::Mat4& mvpMatrix, const GraphicCore::Rect & /*region*/, float positionZ, const GraphicCore::Rect crop, GraphicCore::Color4F color,
                           const GraphicCore::BlendFunc& blend, VideoRotation rotation, GraphicCore::Drawable2D::FlipMode flipMode) = 0;
    virtual const GraphicCore::Texture2D* getOutputTexture() { return nullptr; }
    virtual void release() = 0;
    
protected:
    const VideoTarget *_videoTarget;
};

#endif /* EFFECTEDITOR_VIDEOFRAME_DRAWER_H */
