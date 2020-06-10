//
//  GcImageNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcImageNode.h"

GRAPHICCORE_BEGIN

ImageNode::ImageNode(std::string filePath) {
    FILE* imgfile = fopen(filePath.c_str(), "rb");
    if (imgfile) {
        _image = std::shared_ptr<Image>(Image::createFromFile(imgfile));
        if (_image.get()){
            _width = _image->width();
            _height = _image->height();
        }
        fclose(imgfile);
    }
}

ImageNode::ImageNode(uint8_t * iamge_buffer){
    //TODO: load image form buffer
}

bool ImageNode::createRes() {
    //TODO:
    if (_image) {
        _texture2d = GeneralTexture2D::createFromImage(_image.get());
        _image.reset();
    }
    return TextureNode::createRes();
}
void ImageNode::releaseRes() {
    //TODO:
    if (_texture2d) {
        _texture2d->release();
        delete _texture2d;
        _texture2d = nullptr;
    }
    TextureNode::releaseRes();
}

GRAPHICCORE_END
