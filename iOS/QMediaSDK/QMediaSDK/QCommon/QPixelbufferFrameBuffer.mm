
#include "Utils/Comm.h"
#include "RenderEngine/opengl/render_device_gl.h"
#include "RenderEngine/opengl/TextureFrameBuffer.h"
#include <CoreVideo/CoreVideo.h>
#include <CoreMedia/CoreMedia.h>
#import <CoreFoundation/CoreFoundation.h>
#import "IOSTexture.h"

using QMedia::RenderEngine::Texture2D;
using QMedia::RenderEngine::TextureFrameBuffer;
using QMedia::RenderEngine::GetGlErrorString;

class PixelbufferTexture : public Texture2D {
public:
    PixelbufferTexture(int width, int height) {
        CGSize size = {static_cast<CGFloat>(width), static_cast<CGFloat>(height)};
        ios_texture_ = [[IOSFastTexture alloc] initWithContext:[EAGLContext currentContext] size:size];
        _textureid = ios_texture_.glTexid;
        _textureTarget = ios_texture_.glTexTarget;
        _format = Texture2D::RGBA;
        _width = width;
        _height = height;
    }
    virtual ~PixelbufferTexture() {}
    
    virtual GLuint getTextureId() const override { return _textureid; }
    virtual GLenum getTextureTarget() const override { return _textureTarget; }
    virtual Format getFormat() const override { return _format; }
    virtual int width() const override { return _width;}
    virtual int height() const override { return _height;}

    virtual void release() override { ios_texture_ = nil; }
    
protected:
    IOSFastTexture * ios_texture_;
};

class PixelbufferFrameBuffer : public  TextureFrameBuffer {
public:
    static TextureFrameBuffer* create(int width, int height, bool enable_depth, bool use_multisample) {
        PixelbufferFrameBuffer* frame_buffer = new PixelbufferFrameBuffer();
        frame_buffer->pixelbuffer_texture_ = std::unique_ptr<PixelbufferTexture>(new PixelbufferTexture(width, height));
        CHECK_GL_ERROR(glGenFramebuffers(1, &frame_buffer->fbo_));
        CHECK_GL_ERROR(frame_buffer->use());
        CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        frame_buffer->pixelbuffer_texture_->getTextureId(), 0));
        const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            NSLog(@"glCheckFramebufferStatus: %d", status);
            frame_buffer->restore();
            delete frame_buffer;
            return nullptr;
        }
        frame_buffer->restore();
        return frame_buffer;
    }
    ~PixelbufferFrameBuffer() {
        release();
    }
    void release() override {
        pixelbuffer_texture_.get();
        if (fbo_) {
            glDeleteFramebuffers(1, &fbo_);
            fbo_ = 0;
        }
    }
    bool use() override {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&prev_fbo_);
        if (prev_fbo_ != fbo_) {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        }
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
    bool restore() override {
        glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo_);
        return true;
    }
    
    int width() override { return pixelbuffer_texture_->width(); }
    int height() override { return pixelbuffer_texture_->height(); }
    
    GLuint getFrameBufferId() const override { return fbo_;}
    const Texture2D* getTexture() const override { return pixelbuffer_texture_.get();}
    virtual const Texture2D* getDepTexture() const override { return nullptr;}
    
    virtual void* nativeTexture() override { return pixelbuffer_texture_.get(); }
    
private:
    PixelbufferFrameBuffer() = default;
    
    GLuint fbo_ = 0;
    GLuint prev_fbo_ = 0;
    std::unique_ptr<PixelbufferTexture> pixelbuffer_texture_;
};

#define ENABLE_HW_TEXTURE 0
TextureFrameBuffer* TextureFrameBuffer::createNew(int width, int height, bool enable_depth, bool use_multisample) {
    if ([QGLContext supportsFastTextureUpload]) {
        return PixelbufferFrameBuffer::create(width, height, enable_depth, use_multisample);
    } else {
        return GeneralTextureFrameBuffer::create(width, height, enable_depth, use_multisample);
    }
}
