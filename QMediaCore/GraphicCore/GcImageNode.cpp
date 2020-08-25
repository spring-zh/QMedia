//
//  GcImageNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcImageNode.h"

GRAPHICCORE_BEGIN

ImageNode::ImageNode(std::string filePath) : _texture2d(nullptr){
    _image = std::shared_ptr<Image>(Image::createFromFile(filePath.c_str()));
    if (_image.get()){
        _width = _image->width();
        _height = _image->height();
    }
}

ImageNode::ImageNode(uint8_t * iamge_buffer, int image_size){
    //TODO: load image form buffer
    _image = std::shared_ptr<Image>(Image::createFromBuffer(iamge_buffer, image_size));
    if (_image.get()){
        _width = _image->width();
        _height = _image->height();
    }
}

void ImageNode::draw(GraphicCore::Scene* scene, const Mat4 & transform, uint32_t flags) {
    const Texture2D* texture = _texture2d.get();
    if (texture && _textureDrawer) {
        
        int64_t duration = scene->getDelta() - _renderRange._start;
        if (_effect_group.has_effect()) {
            RenderNode::drawEffects(duration, scene, transform, texture);
        }else {
            _textureDrawer->draw(texture, scene, transform, this);
        }
    }
}

void ImageNode::duplicateDraw(Scene* scene, const Mat4 & transform, const Node* displayNode) {
    if (_texture2d && _textureDrawer) {
        _textureDrawer->draw(_texture2d.get(), scene, transform, displayNode);
    }
}

const Texture2D* ImageNode::getOutputTexture() {
    return _texture2d.get();
}

bool ImageNode::createRes() {
    //TODO:
    if (_image) {
        _texture2d = std::shared_ptr<Texture2D>(GeneralTexture2D::createFromImage(_image.get()));
        _image.reset();
    }
    return RenderNode::createRes();
}
void ImageNode::releaseRes() {
    //TODO:
    _texture2d.reset();
    RenderNode::releaseRes();
}

GRAPHICCORE_END
