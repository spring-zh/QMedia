//
//  GLEngine.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_GLENGINE_H
#define GRAPHICCORE_GLENGINE_H

#include <map>
#include <string>
#include <list>
#include <stack>

#include "GraphicCore/base/ColorTypes.h"
#include "FrameBuffer.h"
#include "Texture2D.h"

GRAPHICCORE_BEGIN

//TODO: implement this
class GLRef {
public:
    virtual void retain() = 0;
    virtual void release() = 0;
};

class GLEngine {
public:

    GLEngine();
    ~GLEngine();

    void getViewPort(Rect &vp);
    void setViewPort(Rect vp);

    Color4F getClearColor();
    void setClearColor(Color4F color);
    void clearByColor();
    void setEnableDepthTest(bool enable);
    bool enableDepthTest() {return _enableDepthTest; }
    
    // save/recover previous render status
    void saveStatus();
    void recoverStatus();

    static bool checkSupportExtension(const char* extension);
    static float GetVersion();

    FrameBuffer* getFramebuffer(const char* name);
    void releaseFramebuffer(FrameBuffer *frameBuffer);

    FrameBuffer* getCurrentFrameBuffer();
    void setCurrentFrameBuffer(FrameBuffer* frameBuffer);
//    void setDefaultFrameBuffer(FrameBuffer* frameBuffer);
    
    void useAsDefaultFrameBuffer();

    void releaseAll();

protected:
    bool _enableDepthTest;
    std::stack<Rect> _viewPortStack;
    std::stack<Color4F> _colorStack;
    std::stack<FrameBuffer*> _framebufferStack;

    std::map<std::string, Texture2D*> _imagTextures;
    std::list<Texture2D*> _cacheTextures;

    std::list<FrameBuffer*> _cacheFramebuffers;

    FrameBuffer *_currentFramebuffer;
    FrameBuffer *_defaultFramebuffer;
};

GLenum glCheckError_(const char* file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)

GRAPHICCORE_END

#endif //GRAPHICCORE_GLENGINE_H
