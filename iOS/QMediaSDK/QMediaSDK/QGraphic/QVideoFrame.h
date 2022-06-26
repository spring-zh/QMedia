//
//  QVideoFrame.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <memory>
#include <CoreVideo/CoreVideo.h>
#include <CoreMedia/CoreMedia.h>
#include "RenderEngine/opengl/Texture2DDrawer.h"
#include "MediaCore/core/VideoFrame.h"
#include "EffectEditor/VideoFrameDrawer.h"

using namespace QMedia;
using QMedia::RenderEngine::ShaderProgram;

//VideoFrameBuffer implemention
class PixelFrameBuffer: public VideoFrameBuffer {
public:
    
    explicit PixelFrameBuffer(CMSampleBufferRef sampleBuffer);
    ~PixelFrameBuffer();

    virtual int width() const override ;
    virtual int height() const override ;

    virtual const uint8_t* DataY() const override ;
    virtual const uint8_t* DataU() const override ;
    virtual const uint8_t* DataV() const override ;

    virtual int StrideY() const override ;
    virtual int StrideU() const override ;
    virtual int StrideV() const override ;

    virtual void* native_handle() const override ;

    virtual std::shared_ptr<VideoFrameBuffer> NativeToI420Buffer() override ;
    
    int64_t _time_stamp;
    int _data_size;
    CMPixelFormatType _formatType;
    
protected:
//    CMSampleBufferRef _sampleBuffer;
    CVImageBufferRef _imageBuffer;
    int _width;
    int _heigth;
};


class PixelFrameNV12Drawer: public VideoFrameDrawer {
public:
    PixelFrameNV12Drawer() = default;
    PixelFrameNV12Drawer(const VideoTarget *videoTarget);
    ~PixelFrameNV12Drawer();

    virtual bool setFrame(const VideoFrame& videoFrame) override;
    void drawFrame(const RenderEngine::Scene* /*scene*/, const RenderEngine::Mat4 & /*transform*/, Api::SceneNode* node) override;

    virtual void drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & /*region*/, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
    const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode = RenderEngine::Drawable2D::NONE) override;
    virtual void release() override ;
private:
    
    void clearTexture();
    
    CVOpenGLESTextureCacheRef _videoTextureCache;
    CVOpenGLESTextureRef _lumaTexture;
    CVOpenGLESTextureRef _chromaTexture;
    
    GLuint _textures[2];
    GLfloat *_colorConversionMatrix;
    
    std::shared_ptr<ShaderProgram> _program;
    
    VideoRotation _rotation;
};

class PixelFrameBGRADrawer: public VideoFrameDrawer {
public:
    PixelFrameBGRADrawer(const VideoTarget *videoTarget);
    ~PixelFrameBGRADrawer();

    virtual bool setFrame(const VideoFrame& videoFrame) override;
    void drawFrame(const RenderEngine::Scene* scene, const RenderEngine::Mat4 & transform, Api::SceneNode* node) override {}
    
    virtual void drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & /*region*/, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
    const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode) override;
    const RenderEngine::Texture2D* getOutputTexture() override;
    virtual void release() override ;
private:
    
    void clearTexture();
    
    CVOpenGLESTextureCacheRef _videoTextureCache;
    CVOpenGLESTextureRef _textureRef;
    
    GLuint _textures;
    GLfloat *_colorConversionMatrix;
    
    std::shared_ptr<RenderEngine::Texture2DDrawer> _textureDrawer;
    RenderEngine::DuplicateTexture2D _duplicateTexture;
    
    VideoRotation _rotation;
};
