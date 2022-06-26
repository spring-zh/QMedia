//
//  frame_buffer_gl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "frame_buffer_gl.h"
#include "RenderEngine/opengl/render_device_gl.h"

#if __ANDROID_NDK__
#include "EGL/egl.h"
/* OpenGL ES extension functions. */
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC glFramebufferTexture2DMultisampleEXT_ = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEIMGPROC glRenderbufferStorageMultisampleEXT_ = NULL;
#else
#define glFramebufferTexture2DMultisampleEXT_ glFramebufferTexture2DMultisampleEXT
#define glRenderbufferStorageMultisampleEXT_ glRenderbufferStorageMultisampleEXT
#endif

namespace QMedia {
namespace RenderEngine {

FrameBufferGL::FrameBufferGL():
_attachTexture(nullptr),
_depthTexture(nullptr),
fbo_gl_(0),
prev_fbol_(0),
_useMultisample(false)
{

}
FrameBufferGL::~FrameBufferGL()
{
    release();
}

FrameBufferGL* FrameBufferGL::createNew()
{
    FrameBufferGL* frameBuffer = new FrameBufferGL();
    glGenFramebuffers(1, &frameBuffer->fbo_gl_);
    return frameBuffer;
}

FrameBufferGL* FrameBufferGL::createCurrent()
{
    GLint currentfbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentfbo);
    FrameBufferGL* frameBuffer = new FrameBufferGL();
    frameBuffer->fbo_gl_ = currentfbo;
    return frameBuffer;
}

FrameBufferGL* FrameBufferGL::createExist(GLuint fbo)
{
    FrameBufferGL* frameBuffer = new FrameBufferGL();
    frameBuffer->fbo_gl_ = fbo;
    return frameBuffer;
}

void FrameBufferGL::use()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbol_);
    if (prev_fbol_ != fbo_gl_)
    {
        CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, fbo_gl_));
    }
}

void FrameBufferGL::release()
{
    if (fbo_gl_) {
        glDeleteFramebuffers(1, &fbo_gl_);
        fbo_gl_ = 0;
    }
}

bool FrameBufferGL::attachTexture2D(AttachMode mode, const Texture2D *texture2D)
{
    use();
    GLuint attachment;
    switch (mode){
        case COLOR:
            attachment = GL_COLOR_ATTACHMENT0;
            break;
        case DEPTH:
            attachment = GL_DEPTH_ATTACHMENT;
            break;
        case STENCIL:
            attachment = GL_STENCIL_ATTACHMENT;
            break;
    }
    if (texture2D == nullptr){
        glFramebufferTexture2D(GL_FRAMEBUFFER,attachment,GL_TEXTURE_2D,0,0);
    }
    else {
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
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D,
                                   texture2D->getTextureId(), 0);
    }

    if (mode == DEPTH) {
        _depthTexture = texture2D;
    } else
        _attachTexture = texture2D;

    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOGW("glCheckFramebufferStatus: %d", status);
        return false;
    }
    return true;
}

}//End RenderEngine
}//End QMedia
