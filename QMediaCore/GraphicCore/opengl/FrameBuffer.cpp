//
//  FrameBuffer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FrameBuffer.h"
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

FrameBuffer::FrameBuffer():
_attachTexture(nullptr),
_depthTexture(nullptr),
_fbo(0),
_last_fbol(0),
_useMultisample(false)
{

}
FrameBuffer::~FrameBuffer()
{
    release();
}

FrameBuffer* FrameBuffer::createNew()
{
    FrameBuffer* frameBuffer = new FrameBuffer();
    glGenFramebuffers(1, &frameBuffer->_fbo);
    return frameBuffer;
}

FrameBuffer* FrameBuffer::createCurrent()
{
    GLint currentfbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentfbo);
    FrameBuffer* frameBuffer = new FrameBuffer();
    frameBuffer->_fbo = currentfbo;
    return frameBuffer;
}

FrameBuffer* FrameBuffer::createExist(GLuint fbo)
{
    FrameBuffer* frameBuffer = new FrameBuffer();
    frameBuffer->_fbo = fbo;
    return frameBuffer;
}

void FrameBuffer::use()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_last_fbol);
    if (_last_fbol != _fbo)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glCheckError() ;
    }
}

void FrameBuffer::release()
{
    if (_fbo) {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
}

bool FrameBuffer::attachTexture2D(AttachMode mode, const Texture2D *texture2D)
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

GRAPHICCORE_END
