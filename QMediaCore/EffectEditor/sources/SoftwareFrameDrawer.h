//
//  SoftwareFrameDrawer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QMEDIASDK_SOFTWAREFRAMEDRAWER_H
#define QMEDIASDK_SOFTWAREFRAMEDRAWER_H

#include "RenderEngine/opengl/shader_program_gl.h"
#include "RenderEngine/opengl/Texture2DDrawer.h"
#include "MediaCore/core/VideoFrame.h"
#include "EffectEditor/VideoFrameDrawer.h"

using QMedia::RenderEngine::ShaderProgram;

class YUVFrameDrawer : public VideoFrameDrawer {
public:
    explicit YUVFrameDrawer(const VideoTarget *videoTarget);
    ~YUVFrameDrawer() = default;

    bool setFrame(const VideoFrame &videoFrame) override;
    void drawFrame(const RenderEngine::Scene* /*scene*/, const RenderEngine::Mat4 & /*transform*/, QMedia::Api::SceneNode* node) override { }

    virtual void drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & /*region*/, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
                           const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode = RenderEngine::Drawable2D::NONE) override ;
    virtual void release() override;

protected:
    VideoRotation _rotation;
    std::shared_ptr<ShaderProgram> _program;
    unsigned int _textureY, _textureU, _textureV;
    int _texWidth, _texHeight;
};

class RGBAFrameDrawer: public VideoFrameDrawer {
public:
    explicit RGBAFrameDrawer(const VideoTarget *videoTarget);
    ~RGBAFrameDrawer();

    virtual bool setFrame(const VideoFrame& videoFrame) override;
    void drawFrame(const RenderEngine::Scene* /*scene*/, const RenderEngine::Mat4 & /*transform*/, QMedia::Api::SceneNode* node) override { }
    virtual void drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & /*region*/, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
                           const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode = RenderEngine::Drawable2D::NONE) override;
    const RenderEngine::Texture2D* getOutputTexture() override { return &_duplicateTexture; }
    virtual void release() override ;
private:

    GLuint _texture;
    int _texWidth, _texHeight;

    std::shared_ptr<RenderEngine::Texture2DDrawer> _textureDrawer;
    RenderEngine::DuplicateTexture2D _duplicateTexture;
};

#endif //QMEDIASDK_SOFTWAREFRAMEDRAWER_H
