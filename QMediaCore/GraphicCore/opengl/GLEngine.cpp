//
//  GLEngine.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GLEngine.h"
#include <algorithm>

#include <iostream>
#include <string>

#include "Utils/Logger.h"

GRAPHICCORE_BEGIN

GLEngine::GLEngine():
_currentFramebuffer(nullptr),
_defaultFramebuffer(nullptr),
_enableDepthTest(false)
{

}
GLEngine::~GLEngine()
{
    
}

void GLEngine::getViewPort(Rect &rt)
{
    int view_port[4];
    glGetIntegerv(GL_VIEWPORT,view_port);
    rt.setRect(view_port[0], view_port[1], view_port[2], view_port[3]);
}
void GLEngine::setViewPort(Rect rt)
{
    glViewport(rt.getMinX(),rt.getMinY(),rt.size.width,rt.size.height);
}

Color4F GLEngine::getClearColor()
{
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    return Color4F(clearColor[0],clearColor[1],clearColor[2],clearColor[3]);
}

void GLEngine::setClearColor(Color4F color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void GLEngine::clearByColor()
{
//    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void GLEngine::setEnableDepthTest(bool enable)
{
    _enableDepthTest = enable;
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0, 1);
    glClearDepthf(1.0f);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
    if (enable)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void GLEngine::saveStatus()
{
    _colorStack.push(getClearColor());
    Rect viewPort;
    getViewPort(viewPort);
    _viewPortStack.push(viewPort);
    _framebufferStack.push(getCurrentFrameBuffer());
}
void GLEngine::recoverStatus()
{
    setClearColor(_colorStack.top());
    _colorStack.pop();
    setViewPort(_viewPortStack.top());
    _viewPortStack.pop();
    setCurrentFrameBuffer(_framebufferStack.top());
    _framebufferStack.pop();
}

bool GLEngine::checkSupportExtension(const char* extension)
{
    std::string extensions = (const char*)glGetString(GL_EXTENSIONS);
    return (extensions.find(extension)!= std::string::npos);
}

FrameBuffer* GLEngine::getCurrentFrameBuffer() {
    return _currentFramebuffer;
}

void GLEngine::setCurrentFrameBuffer(FrameBuffer* frameBuffer)
{
    if(frameBuffer != nullptr)
        _currentFramebuffer = frameBuffer;
    else
        _currentFramebuffer = _defaultFramebuffer;
    _currentFramebuffer->use();
}

//void GLEngine::setDefaultFrameBuffer(FrameBuffer* frameBuffer){
//
//    if (_defaultFramebuffer) {
//        delete _defaultFramebuffer;
//    }
//    _defaultFramebuffer = frameBuffer;
////    if (_currentFramebuffer == nullptr) {
//         _currentFramebuffer = frameBuffer;
////    }
//}

void GLEngine::useAsDefaultFrameBuffer()
{
    GLint currentfbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentfbo);
    if (_defaultFramebuffer) {
        if (_defaultFramebuffer->_fbo != currentfbo) {
            delete _defaultFramebuffer;
            _defaultFramebuffer = nullptr;
        }
        else
            return;
    }
    _defaultFramebuffer = FrameBuffer::createExist(currentfbo);
    _currentFramebuffer = _defaultFramebuffer;
}

FrameBuffer* GLEngine::getFramebuffer(const char* name)
{
    FrameBuffer* frameBuffer = FrameBuffer::createNew();
    _cacheFramebuffers.push_back(frameBuffer);
    return frameBuffer;
}
void GLEngine::releaseFramebuffer(FrameBuffer *frameBuffer)
{
    if(frameBuffer == nullptr)
        return;
    auto i_find = std::find(_cacheFramebuffers.begin(),_cacheFramebuffers.end(),frameBuffer);
    if (i_find != _cacheFramebuffers.end()){
        _cacheFramebuffers.erase(i_find);
        if (frameBuffer == _currentFramebuffer){

        }
        frameBuffer->release();
    }
}

void GLEngine::releaseAll()
{

}

GLenum glCheckError_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR) {
        std::string error;
        switch (errorCode) {
        case GL_INVALID_ENUM:
            error = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error = "INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            error = "INVALID_OPERATION";
            break;
        //case GL_STACK_OVERFLOW:
        //  error = "STACK_OVERFLOW";
        //  break;
        //case GL_STACK_UNDERFLOW:
        //  error = "STACK_UNDERFLOOR";
        //  break;
        case GL_OUT_OF_MEMORY:
            error = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }

        LOGW("glCheckError: %s", error.c_str());
//        std::cout << error << " | " << file << " (" << line << ") " << std::endl;
    }
    
    return errorCode;
}

GRAPHICCORE_END
