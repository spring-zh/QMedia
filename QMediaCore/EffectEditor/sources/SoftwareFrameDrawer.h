//
//  SoftwareFrameDrawer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QMEDIASDK_SOFTWAREFRAMEDRAWER_H
#define QMEDIASDK_SOFTWAREFRAMEDRAWER_H

#include "GraphicCore/opengl/ShaderProgram.h"
#include "MediaCore/core/VideoFrame.h"
#include "GraphicCore/opengl/Texture2DDrawer.h"
#include "EffectEditor/VideoFrameDrawer.h"

using GraphicCore::ShaderProgram;

class YUVFrameDrawer : public VideoFrameDrawer {
public:
    explicit YUVFrameDrawer(const VideoTarget *videoTarget);
    ~YUVFrameDrawer() = default;

    bool setFrame(const VideoFrame &videoFrame) override;
    void drawFrame(const GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, QMedia::Api::SceneNode* node) override { }

    virtual void drawFrame(const GraphicCore::Mat4& mvpMatrix, const GraphicCore::Rect & /*region*/, float positionZ, const GraphicCore::Rect crop, GraphicCore::Color4F color,
                           const GraphicCore::BlendFunc& blend, VideoRotation rotation, GraphicCore::Drawable2D::FlipMode flipMode = GraphicCore::Drawable2D::NONE) override ;
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
    void drawFrame(const GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, QMedia::Api::SceneNode* node) override { }
    virtual void drawFrame(const GraphicCore::Mat4& mvpMatrix, const GraphicCore::Rect & /*region*/, float positionZ, const GraphicCore::Rect crop, GraphicCore::Color4F color,
                           const GraphicCore::BlendFunc& blend, VideoRotation rotation, GraphicCore::Drawable2D::FlipMode flipMode = GraphicCore::Drawable2D::NONE) override;
    const GraphicCore::Texture2D* getOutputTexture() override { return &_duplicateTexture; }
    virtual void release() override ;
private:

    GLuint _texture;
    int _texWidth, _texHeight;

    std::shared_ptr<GraphicCore::Texture2DDrawer> _textureDrawer;
    GraphicCore::DuplicateTexture2D _duplicateTexture;
};

#endif //QMEDIASDK_SOFTWAREFRAMEDRAWER_H
