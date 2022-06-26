//
//  TextureFrameBuffer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "TextureFrameBuffer.h"
#include "GraphicCore/opengl/GLEngine.h"
#include "Utils/Logger.h"

#if __ANDROID_NDK__
#include "EGL/egl.h"
/* OpenGL ES extension functions. */
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC glFramebufferTexture2DMultisampleEXT_ = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC glRenderbufferStorageMultisampleEXT_ = NULL;
#else
#define glFramebufferTexture2DMultisampleEXT_ glFramebufferTexture2DMultisampleEXT
#define glRenderbufferStorageMultisampleEXT_ glRenderbufferStorageMultisampleEXT
#endif

GRAPHICCORE_BEGIN

TextureFrameBuffer* GeneralTextureFrameBuffer::create(int width, int height, bool enable_depth, bool use_multisample) {
    auto texture_framebuffer = new GeneralTextureFrameBuffer();
    texture_framebuffer->enable_depth_ = enable_depth;
    texture_framebuffer->use_multisample_ = use_multisample;
    texture_framebuffer->width_ = width;
    texture_framebuffer->height_ = height;
    
    glGenFramebuffers(1, &texture_framebuffer->fbo_);
    texture_framebuffer->attach_texture_ = std::unique_ptr<Texture2D>(GeneralTexture2D::createTexture(Texture2D::RGBA, width, height));
    texture_framebuffer->use();
#if __ANDROID_NDK__
    bool uspport_multisample = GLEngine::checkSupportExtension("GL_EXT_multisampled_render_to_texture");
    glFramebufferTexture2DMultisampleEXT_ = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC) eglGetProcAddress(
            "glFramebufferTexture2DMultisampleEXT");
    if (!glFramebufferTexture2DMultisampleEXT_) {
        LOGW("Couldn't get function pointer to glFramebufferTexture2DMultisampleEXT()");
        uspport_multisample = false;
    }

    if (uspport_multisample && _useMultisample) {
        GLint samples = 0;
        glGetIntegerv(GL_MAX_SAMPLES_EXT, &samples);
        if (samples > 4 || samples <= 0)
        {
            samples = 4;
        }
        glFramebufferTexture2DMultisampleEXT_(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D,
                                             texture2D->getTextureId(), 0, samples);
        GLenum  err = glGetError();
        if (err) {
            glCheckError();
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture2D->getTextureId(), 0);
        }
    } else
#endif
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                               texture_framebuffer->attach_texture_->getTextureId(), 0);

    if (texture_framebuffer->enable_depth_) {
        texture_framebuffer->depth_texture_ = std::unique_ptr<Texture2D>(GeneralTexture2D::createTexture(Texture2D::DEPTH, width, height));
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        texture_framebuffer->depth_texture_->getTextureId(), 0);
    }
    
    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOGW("glCheckFramebufferStatus: %d", status);
        texture_framebuffer->restore();
        delete texture_framebuffer;
        return nullptr;
    }
    
    return texture_framebuffer;
}

GeneralTextureFrameBuffer::~GeneralTextureFrameBuffer() {
    release();
}

void GeneralTextureFrameBuffer::release() {
    attach_texture_.reset();
    depth_texture_.reset();
    if (fbo_) {
        glDeleteFramebuffers(1, &fbo_);
        fbo_ = 0;
    }
}
bool GeneralTextureFrameBuffer::use() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo_);
    if (prev_fbo_ != fbo_) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glCheckError() ;
    }
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}
bool GeneralTextureFrameBuffer::restore() {
    glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo_);
    return true;
}

int GeneralTextureFrameBuffer::width() {
    return width_;
}
int GeneralTextureFrameBuffer::height() {
    return height_;
}

GLuint GeneralTextureFrameBuffer::getFrameBufferId() const {
    return fbo_;
}
const Texture2D* GeneralTextureFrameBuffer::getTexture() const {
    return attach_texture_.get();
}
const Texture2D* GeneralTextureFrameBuffer::getDepTexture() const {
    return depth_texture_.get();
}

void* GeneralTextureFrameBuffer::nativeTexture() {
    return attach_texture_.get();
}

GRAPHICCORE_END
