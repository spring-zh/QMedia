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
#include "RenderEngine/opengl/texture2d_gl.h"
#include "RenderEngine/SceneNode.h"
#include "RenderEngine/RenderScene.h"
#include "MediaCore/core/VideoFrame.h"
#include "MediaCore/output/VideoTarget.h"

using namespace QMedia;

class VideoFrameDrawer{
public:
    static VideoFrameDrawer* createVideoFrameDrawer(RawVideoFormat pixel_format);
    
    VideoFrameDrawer():_videoTarget(nullptr) {}
    VideoFrameDrawer(const VideoTarget *videoTarget):_videoTarget(videoTarget) {}
    virtual ~VideoFrameDrawer(){}

    virtual bool setFrame(const VideoFrame& videoFrame) = 0;
    virtual void drawFrame(const RenderEngine::Scene* scene, const RenderEngine::Mat4 & transform, Api::SceneNode* node) = 0;
    virtual void drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & /*region*/, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
                           const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode) = 0;
    virtual const RenderEngine::Texture2D* getOutputTexture() { return nullptr; }
    virtual void release() = 0;
    
protected:
    const VideoTarget *_videoTarget;
};

#endif /* EFFECTEDITOR_VIDEOFRAME_DRAWER_H */
