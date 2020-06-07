//
//  GcTextureNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_TEXTURENODE_H
#define GRAPHICCORE_TEXTURENODE_H

#include "GraphicCore/GcRenderNode.h"
#include "GraphicCore/opengl/GLEngine.h"
#include "GraphicCore/opengl/ShaderProgram.h"

GRAPHICCORE_BEGIN

class TextureNode : public RenderNode{
public:

    TextureNode();
    virtual ~TextureNode();

    void setSourceTexture(Texture2D * texture2D) { _texture2d = texture2D; }
    
    //call by DuplicateNode
    virtual void duplicateDraw(Scene* /*scene*/, const Mat4 & /*transform*/, const Node* /*diaplayNode*/) override ;
    
    // create/release gl resource
    virtual bool createRes() override ;
    virtual void releaseRes() override ;

protected:

    Texture2D *_texture2d;
    ShaderProgram _shaderProgram;
};

GRAPHICCORE_END

#endif /* GRAPHICCORE_TEXTURENODE_H */
