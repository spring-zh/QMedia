//
//  GcImageNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_IMAGENODE_H
#define GRAPHICCORE_IMAGENODE_H

#include "GraphicCore/GcRenderNode.h"
#include "GraphicCore/base/Image.h"
#include "opengl/Texture2DDrawer.h"

GRAPHICCORE_BEGIN

class ImageNode : public RenderNode{
public:
    explicit ImageNode(std::string filePath);
    explicit ImageNode(uint8_t * iamge_buffer, int image_size);
    virtual ~ImageNode() = default;

    int getWidth() { return _width; }
    int getHeight() { return _height; }

    //call by DuplicateNode
    virtual void duplicateDraw(Scene* /*scene*/, const Mat4 & /*transform*/, const Node* /*displayNode*/) override ;
    
    // create/release gl resource
    virtual bool createRes() override ;
    virtual void releaseRes() override ;

private:
    std::shared_ptr<Image> _image;
    Texture2D *_texture2d;
    std::shared_ptr<Texture2DDrawer> _textureDrawer;
    int _width;
    int _height;
};

using ImageNodeRef = std::shared_ptr<ImageNode>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_IMAGENODE_H */
