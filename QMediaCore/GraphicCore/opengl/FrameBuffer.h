//
//  FrameBuffer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_FRAMEBUFFER_H
#define GRAPHICCORE_FRAMEBUFFER_H

#include "Texture2D.h"

GRAPHICCORE_BEGIN

class FrameBuffer final {
public:
    
    ~FrameBuffer();

    static FrameBuffer* createNew();
    static FrameBuffer* createCurrent();
    static FrameBuffer* createExist(GLuint fbo);

    enum AttachMode{
        COLOR,
        DEPTH,
        STENCIL
    };

    void use();
    void release();
    GLuint getFrameBufferId() const { return _fbo; }
    bool attachTexture2D(AttachMode mode, Texture2D *texture2D);
    Texture2D *getAttachTexture2D() const { return _attachTexture;};

    void useMultisample(bool used) { _useMultisample = used; }

protected:
    friend class GLEngine;
    FrameBuffer();

    Texture2D *_attachTexture;
    GLuint _fbo;
    GLint _last_fbol;
    bool _useMultisample;
};

GRAPHICCORE_END

#endif //GRAPHICCORE_FRAMEBUFFER_H
