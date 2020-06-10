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
 * must use in opengl thread
 */
class Texture2DDrawer {
public:

    Texture2DDrawer();
    explicit Texture2DDrawer(const char* vshader, const char* fshader);
    virtual ~Texture2DDrawer();

    virtual void draw(const Texture2D* texture,const Scene* scene, const Mat4 & /*transform*/, const Node* /*displayNode*/, Drawable2D::FlipMode flipMode = Drawable2D::NONE) ;

protected:
    ShaderProgram _shaderProgram;
};

GRAPHICCORE_END

#endif /* GRAPHICCORE_TEXTURE2D_DRAWER_H */
