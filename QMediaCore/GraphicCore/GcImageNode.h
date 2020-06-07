//
//  GcImageNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_IMAGENODE_H
#define GRAPHICCORE_IMAGENODE_H

#include "GraphicCore/GcTextureNode.h"
#include "GraphicCore/base/Image.h"

GRAPHICCORE_BEGIN

class ImageNode : public TextureNode{
public:
    explicit ImageNode(std::string filePath);
    explicit ImageNode(uint8_t * iamge_buffer);
    virtual ~ImageNode() = default;
    
    // create/release gl resource
    virtual bool createRes() override ;
    virtual void releaseRes() override ;

private:
    std::shared_ptr<Image> _image;
};

using ImageNodeRef = std::shared_ptr<ImageNode>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_IMAGENODE_H */
