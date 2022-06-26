//
//  render_device_gl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "render_device_gl.h"
#include <algorithm>

#include <iostream>
#include <string>

namespace QMedia {
namespace RenderEngine {

RenderDeviceGL::UPtr RenderDeviceGL::createDevice(int type) {
  return RenderDeviceGL::UPtr(new RenderDeviceGL());
}

RenderDeviceGL::RenderDeviceGL():
current_framebuffer_(nullptr),
default_framebuffer_(nullptr),
enable_depth_test_(false) {

}
RenderDeviceGL::~RenderDeviceGL() {
    
}

void RenderDeviceGL::getViewPort(Api::Vec4f &rt)
{
    int view_port[4];
    glGetIntegerv(GL_VIEWPORT,view_port);
    rt = Api::Vec4f(view_port[0], view_port[1], view_port[2], view_port[3]);
}
void RenderDeviceGL::setViewPort(Api::Vec4f rt)
{
    glViewport(rt.v1,rt.v2,rt.v3,rt.v4);
}

Api::Vec4f RenderDeviceGL::getClearColor()
{
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    return Api::Vec4f(clearColor[0],clearColor[1],clearColor[2],clearColor[3]);
}

void RenderDeviceGL::setClearColor(Api::Vec4f color)
{
    glClearColor(color.v1, color.v2, color.v3, color.v4);
}

void RenderDeviceGL::clearByColor()
{
//    glClearColor(r,g,b,a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void RenderDeviceGL::setEnableDepthTest(bool enable)
{
    enable_depth_test_ = enable;
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

void RenderDeviceGL::Finish() {
  glFinish();
}

void RenderDeviceGL::saveStatus()
{
    color_stack_.push(getClearColor());
    Api::Vec4f viewPort;
    getViewPort(viewPort);
    view_port_stack_.push(viewPort);
    framebuffer_stack_.push(getCurrentFrameBuffer());
}
void RenderDeviceGL::recoverStatus()
{
    setClearColor(color_stack_.top());
    color_stack_.pop();
    setViewPort(view_port_stack_.top());
    view_port_stack_.pop();
    setCurrentFrameBuffer(framebuffer_stack_.top());
    framebuffer_stack_.pop();
}

FrameBufferGL* RenderDeviceGL::getCurrentFrameBuffer() {
    return current_framebuffer_;
}

void RenderDeviceGL::setCurrentFrameBuffer(FrameBufferGL* frameBuffer)
{
    if(frameBuffer != nullptr)
      current_framebuffer_ = frameBuffer;
    else
      current_framebuffer_ = default_framebuffer_;
    current_framebuffer_->use();
}

void RenderDeviceGL::useAsDefaultFrameBuffer()
{
    GLint currentfbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentfbo);
    if (default_framebuffer_) {
        if (default_framebuffer_->fbo_gl_ != currentfbo) {
            delete default_framebuffer_;
            default_framebuffer_ = nullptr;
        }
        else
            return;
    }
    default_framebuffer_ = FrameBufferGL::createExist(currentfbo);
    current_framebuffer_ = default_framebuffer_;
}

void RenderDeviceGL::releaseAll() {

}

const char* GetGlErrorString(GLenum error_code) {
    switch (error_code) {
    case GL_INVALID_ENUM:
        return "INVALID_ENUM";
    case GL_INVALID_VALUE:
        return  "INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "INVALID_OPERATION";
    //case GL_STACK_OVERFLOW:
    //  error_str = "STACK_OVERFLOW";
    //  break;
    //case GL_STACK_UNDERFLOW:
    //  error_str = "STACK_UNDERFLOOR";
    //  break;
    case GL_OUT_OF_MEMORY:
        return "OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "INVALID_FRAMEBUFFER_OPERATION";
    }
    return "";
}

#pragma mark -------- static field ----------
bool RenderDeviceGL::checkSupportExtension(const char* extension)
{
    std::string extensions = (const char*)glGetString(GL_EXTENSIONS);
    return (extensions.find(extension)!= std::string::npos);
}

float RenderDeviceGL::GetVersion() {
    auto version = glGetString(GL_VERSION);
    const char *pos = strchr((const char*)version, '.');
    if (pos) {
        return atof(pos - 1);
    }
    return 2.0;
}

}//End RenderEngine
}//End QMedia
