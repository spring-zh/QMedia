//
//  frame_buffer_gl.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef RENDERENGINE_FRAMEBUFFER_GL_H
#define RENDERENGINE_FRAMEBUFFER_GL_H

#include "texture2d_gl.h"

namespace QMedia {
namespace RenderEngine {

class FrameBufferGL final {
public:
    
    ~FrameBufferGL();

    static FrameBufferGL* createNew();
    static FrameBufferGL* createCurrent();
    static FrameBufferGL* createExist(GLuint fbo);

    enum AttachMode{
        COLOR,
        DEPTH,
        STENCIL
    };

    void use();
    void release();
    GLuint getFrameBufferId() const { return fbo_gl_; }
    bool attachTexture2D(AttachMode mode, const Texture2D *texture2D);
    const Texture2D *getAttachTexture2D() const { return _attachTexture;};

    void useMultisample(bool used) { _useMultisample = used; }

protected:
    friend class RenderDeviceGL;
    FrameBufferGL();

    const Texture2D *_attachTexture;
    const Texture2D *_depthTexture;
    GLuint fbo_gl_;
    GLint  prev_fbol_;
    bool _useMultisample;
};

}//End RenderEngine
}//End QMedia

#endif //RENDERENGINE_FRAMEBUFFER_GL_H
