//
//  VideoRenderImpl.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "VideoRenderNodeImpl.h"
#include "MediaSegmentImpl.h"

namespace QMedia { namespace Api {

VideoRenderNodeImpl::VideoRenderNodeImpl(MediaSegmentImpl* segment): graphic_(new MediaGraphicNode(this)), segment_(segment) {
    
}
VideoRenderNodeImpl::~VideoRenderNodeImpl() {
    
}

void VideoRenderNodeImpl::setVisible(bool visible) {
    graphic_->setVisible(visible);
}

void VideoRenderNodeImpl::setAlpha(float alpha) {
   
}

void VideoRenderNodeImpl::setCrop(const Vec4f & crop) {
    graphic_->setCrop(GraphicCore::Rect(crop.v1,crop.v2,crop.v3,crop.v4));
}

void VideoRenderNodeImpl::setContentSize(const Size & content_size) {
    graphic_->setContentSize(GraphicCore::Size(content_size.width, content_size.height));
}

void VideoRenderNodeImpl::setAnchorPoint(const Point & anchor_point) {
    graphic_->setAnchorPoint(GraphicCore::Point(anchor_point.x, anchor_point.y));
}

void VideoRenderNodeImpl::setPosition(const Point & position) {
    graphic_->setPosition(GraphicCore::Point(position.x, position.y));
}

void VideoRenderNodeImpl::setPositionZ(int32_t positionz) {
    graphic_->setPositionZ(positionz);
}

void VideoRenderNodeImpl::setRotation(int32_t rotation) {
    graphic_->setRotation(rotation);
}

void VideoRenderNodeImpl::setRotation3d(const Vec4f & rotation3d) {
    graphic_->setRotation3D(GraphicCore::Vec3(rotation3d.v1, rotation3d.v2, rotation3d.v3));
}

void VideoRenderNodeImpl::setScaleX(float scalex) {
    graphic_->setScaleX(scalex);
}

void VideoRenderNodeImpl::setScaleY(float scaley) {
    graphic_->setScaleY(scaley);
}

void VideoRenderNodeImpl::setScaleZ(float scalez) {
    graphic_->setScaleZ(scalez);
}

std::vector<Animator> VideoRenderNodeImpl::getAnimators() {
    std::vector<Animator> nullopt;
    return nullopt;
}

bool VideoRenderNodeImpl::addAnimator(const Animator & animator) {
    return false;
}

bool VideoRenderNodeImpl::removeAnimator(const Animator & animator) {
    return false;
}


const MediaRange VideoRenderNodeImpl::getRange() const {
    return segment_->getDisplayRange();
}

void VideoRenderNodeImpl::PutVideoFrame(DecodedFrame decodeframe) {
    VideoFrame videoframe(decodeframe.frame_buffer_, decodeframe.timestamp_);
    graphic_->updateFrame(videoframe);
}

VideoRenderNodeImpl::MediaGraphicNode* VideoRenderNodeImpl::getMediaGraphicNode() const {
    return graphic_.get();
}

}
}
