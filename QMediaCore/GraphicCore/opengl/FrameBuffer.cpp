//
//  FrameBuffer.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FrameBuffer.h"
#include "Utils/Logger.h"

GRAPHICCORE_BEGIN

FrameBuffer::FrameBuffer():
_attachTexture(nullptr),
_fbo(0),
_last_fbol(0)
{

}
FrameBuffer::~FrameBuffer()
{

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
    }
}

void FrameBuffer::release()
{
    if (_fbo) {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
}

bool FrameBuffer::attachTexture2D(AttachMode mode, Texture2D *texture2D)
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
    else
        glFramebufferTexture2D(GL_FRAMEBUFFER,attachment,GL_TEXTURE_2D,texture2D->getTextureId(),0);

    _attachTexture = texture2D;

    const int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGW("glCheckFramebufferStatus: %d", status);
        return false;
    }
    return true;
}

GRAPHICCORE_END
