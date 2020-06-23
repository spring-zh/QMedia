//
//  Texture2DDrawer.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_TEXTURE2D_DRAWER_H
#define GRAPHICCORE_TEXTURE2D_DRAWER_H

#include "GraphicCore/GcNode.h"
#include "Drawable2D.h"
#include "GLEngine.h"
#include "ShaderProgram.h"

GRAPHICCORE_BEGIN

/**
 *  note: must use in opengl thread
 *  default vertex and uniform below
 *  ---------------------------------------------------------
 *  addVertexAttribOption("a_position",VertexAttrib::VERTEX3);
 *  addVertexAttribOption("a_texCoord",VertexAttrib::TEXCOORD);
 *  addUniformOption("uMVPMatrix",Uniform::MATRIX4);
 *  addUniformOption("uTexMatrix",Uniform::MATRIX4);
 *  addUniformOption("uTexture",Uniform::TEXTURE);
 *  addUniformOption("uColor",Uniform::FLOAT4);
 */
class Texture2DDrawer {
public:

    Texture2DDrawer();
    explicit Texture2DDrawer(const char* vshader, const char* fshader);
    virtual ~Texture2DDrawer();

    ShaderProgram& getShaderProgram() { return _shaderProgram; }

    virtual void draw(const Texture2D* texture,const Scene* scene, const Mat4 & /*transform*/, const Node* /*node*/, Drawable2D::FlipMode flipMode = Drawable2D::NONE) ;

protected:
    ShaderProgram _shaderProgram;
};

GRAPHICCORE_END

#endif /* GRAPHICCORE_TEXTURE2D_DRAWER_H */
