//
//  Texture2DDrawer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_TEXTURE2D_DRAWER_H
#define GRAPHICCORE_TEXTURE2D_DRAWER_H

#include "RenderEngine/RenderScene.h"
#include "Drawable2D.h"
#include "RenderEngine/opengl/render_device_gl.h"
#include "RenderEngine/opengl/shader_program_gl.h"

namespace QMedia { namespace Api {
class SceneNode;
}
}

namespace QMedia {
namespace RenderEngine {

using Api::Scene;

/**
 *  note: must use in opengl thread
 *  default vertex and uniform below
 *  ---------------------------------------------------------
 *  VertexAttribOption("a_position",VertexAttrib::VERTEX3);
 *  VertexAttribOption("a_texCoord",VertexAttrib::TEXCOORD);
 *  UniformOption("uMVPMatrix",Uniform::MATRIX4);
 *  UniformOption("uTexMatrix",Uniform::MATRIX4);
 *  UniformOption("uTexture",Uniform::TEXTURE);
 *  UniformOption("uColor",Uniform::FLOAT4);
 */
class Texture2DDrawer {
public:

    Texture2DDrawer();
    explicit Texture2DDrawer(const char* vshader, const char* fshader);
    virtual ~Texture2DDrawer();

    ShaderProgram& getShaderProgram() { return _shaderProgram; }

    //draw with node's settings
    
    virtual void draw(const Texture2D* texture,const Scene* scene, const Mat4 & transform, QMedia::Api::SceneNode* node, Drawable2D::FlipMode flipMode = Drawable2D::NONE) ;
    
    virtual void draw(const Texture2D* texture, const Mat4& mvpMatrix, const RenderEngine::Rect & /*region*/, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
                            const BlendFunc& blend, Drawable2D::FlipMode flipMode);

protected:
    ShaderProgram _shaderProgram;
};

}//End RenderEngine
}//End QMedia

#endif /* GRAPHICCORE_TEXTURE2D_DRAWER_H */
