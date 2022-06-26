//
//  render_device_gl.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef RENDERENGINE_RENDERDEVICE_GL_H
#define RENDERENGINE_RENDERDEVICE_GL_H

#include <map>
#include <string>
#include <list>
#include <stack>

#include "vec4f.h"
#include "Utils/Logger.h"
#include "gl_platform.h"
#include "frame_buffer_gl.h"
#include "texture2d_gl.h"

namespace QMedia {
namespace RenderEngine {

class RenderDeviceGL {
public:
  using SPtr = std::shared_ptr<RenderDeviceGL>;
  using UPtr = std::unique_ptr<RenderDeviceGL>;
  ~RenderDeviceGL();

  static UPtr createDevice(int type);
  static float GetVersion();
  static bool checkSupportExtension(const char* extension);

  void getViewPort(Api::Vec4f &vp);
  void setViewPort(Api::Vec4f vp);

  Api::Vec4f getClearColor();
  void setClearColor(Api::Vec4f color);
  void clearByColor();
  void setEnableDepthTest(bool enable);
  bool enableDepthTest() {return enable_depth_test_; }
  void Finish();
  
  // save/recover previous render status
  void saveStatus();
  void recoverStatus();

  FrameBufferGL* getCurrentFrameBuffer();
  void setCurrentFrameBuffer(FrameBufferGL* frameBuffer);
  
  void useAsDefaultFrameBuffer();

  void releaseAll();

protected:
    RenderDeviceGL();
    bool enable_depth_test_;
    std::stack<Api::Vec4f> view_port_stack_;
    std::stack<Api::Vec4f> color_stack_;
    std::stack<FrameBufferGL*> framebuffer_stack_;

    FrameBufferGL *current_framebuffer_;
    FrameBufferGL *default_framebuffer_;
};

extern const char* GetGlErrorString(GLenum error_code);

#define CHECK_GL_ERROR(fun) \
do {\
    fun; \
    GLenum error_code = glGetError();\
    if(error_code != GL_NO_ERROR) { \
        LOGE("CHECK_GL_ERROR (%s) (Error code: 0x%x - %s)\n", /*__FILE_NAME__, __LINE__,*/ #fun, error_code, GetGlErrorString(error_code));\
        assert(0);\
    }\
}while(0);

}//End RenderEngine
}//End QMedia

#endif //RENDERENGINE_RENDERDEVICE_GL_HRENDERDEVICE_GL_H
