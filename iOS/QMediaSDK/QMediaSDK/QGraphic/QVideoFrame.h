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
#include "GraphicCore/opengl/Texture2DDrawer.h"
#include "MediaCore/mediadef/VideoFrame.h"
#include "EffectEditor/VideoFrameDrawer.h"

using GraphicCore::ShaderProgram;

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
    PixelFrameNV12Drawer(const VideoTarget *videoTarget);
    ~PixelFrameNV12Drawer();

    virtual bool setFrame(const VideoFrame& videoFrame) override;
    virtual void drawFrame(const GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, const GraphicCore::Node* node) override;
    virtual void release() override ;
private:
    
    void clearTexture();
    
    CVOpenGLESTextureCacheRef _videoTextureCache;
    CVOpenGLESTextureRef _lumaTexture;
    CVOpenGLESTextureRef _chromaTexture;
    
    GLuint _textures[2];
    GLfloat *_colorConversionMatrix;
    
    std::shared_ptr<ShaderProgram> _program;
};

class PixelFrameBGRADrawer: public VideoFrameDrawer {
public:
    PixelFrameBGRADrawer(const VideoTarget *videoTarget);
    ~PixelFrameBGRADrawer();

    virtual bool setFrame(const VideoFrame& videoFrame) override;
    virtual void drawFrame(const GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, const GraphicCore::Node* node) override;
    const GraphicCore::Texture2D* getOutputTexture() override;
    virtual void release() override ;
private:
    
    void clearTexture();
    
    CVOpenGLESTextureCacheRef _videoTextureCache;
    CVOpenGLESTextureRef _textureRef;
    
    GLuint _textures;
    GLfloat *_colorConversionMatrix;
    
    std::shared_ptr<GraphicCore::Texture2DDrawer> _textureDrawer;
    GraphicCore::DuplicateTexture2D _duplicateTexture;
};
