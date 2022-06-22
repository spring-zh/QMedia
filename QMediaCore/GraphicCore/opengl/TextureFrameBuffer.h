//
//  TextureFrameBuffer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_TEXTURE_FRAMEBUFFER_H
#define GRAPHICCORE_TEXTURE_FRAMEBUFFER_H

#include "Texture2D.h"

GRAPHICCORE_BEGIN

class TextureFrameBuffer {
public:
    static TextureFrameBuffer* createNew(int width, int height, bool enable_depth, bool use_multisample);
    virtual ~TextureFrameBuffer() {}
    virtual void release() = 0;
    virtual bool use() = 0;
    virtual bool restore() = 0;
    
    virtual int width() = 0;
    virtual int height() = 0;
    
    virtual GLuint getFrameBufferId() const = 0;
    virtual const Texture2D* getTexture() const = 0;
    virtual const Texture2D* getDepTexture() const = 0;
    
    virtual void* nativeTexture() = 0;
};

class GeneralTextureFrameBuffer : public TextureFrameBuffer {
public:
    
    static TextureFrameBuffer* create(int width, int height, bool enable_depth, bool use_multisample);
    
    ~GeneralTextureFrameBuffer();

    virtual void release() override;
    virtual bool use() override;
    virtual bool restore() override;
    
    virtual int width() override;
    virtual int height() override;
    
    virtual GLuint getFrameBufferId() const override;
    virtual const Texture2D* getTexture() const override;
    virtual const Texture2D* getDepTexture() const override;
    
    virtual void* nativeTexture() override;

protected:
    friend class GLEngine;
    GeneralTextureFrameBuffer() = default;

    std::unique_ptr<Texture2D> attach_texture_;
    std::unique_ptr<Texture2D> depth_texture_;
    GLuint fbo_ = 0;
    GLint prev_fbo_ = 0;
    bool enable_depth_ = false;
    bool use_multisample_ = false;
    
    int width_ = 0;
    int height_ = 0;
};

GRAPHICCORE_END

#endif //GRAPHICCORE_FRAMEBUFFER_H
