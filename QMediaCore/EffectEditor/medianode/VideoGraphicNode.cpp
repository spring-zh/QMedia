//
//  VideoGraphicNode.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "VideoGraphicNode.h"

namespace QMedia { namespace Api {

VideoGraphicNode::VideoGraphicNode(MediaSegmentImpl* segment):segment_(segment), _lastUpdateTime(-1) {
    _drawer = std::unique_ptr<VideoFrameDrawer>(VideoFrameDrawer::createVideoFrameDrawer(RawVideoFormat::kHardware));
}
VideoGraphicNode::~VideoGraphicNode() {
    _drawer.reset();
}

void VideoGraphicNode::PutVideoFrame(DecodedFrame decodeframe) {
    VideoFrame videoframe(decodeframe.frame_buffer_, decodeframe.timestamp_);
    _drawer->setFrame(videoframe);
}

void VideoGraphicNode::draw(RenderEngine::Scene* scene, const RenderEngine::Mat4 & transform, uint32_t flags) {
    
    int64_t duration = scene->getDelta() - _renderRange._start;
    if (_effect_group.has_effect() && _drawer->getOutputTexture()) {
        const RenderEngine::Texture2D* texture = _drawer->getOutputTexture();
        SceneNode::drawEffects(duration, scene, transform, texture);
    }else {
        _drawer->drawFrame(scene,transform, this);
    }
}

void VideoGraphicNode::duplicateDraw(RenderEngine::Scene* scene, const RenderEngine::Mat4 & transform, SceneNode* diaplayNode) {
    _drawer->drawFrame(scene,transform, diaplayNode);
}

const Range<int64_t> VideoGraphicNode::getRange() {
    auto range = segment_->getDisplayRange();
    return Range<int64_t>(range.start, range.end);
}

void VideoGraphicNode::releaseRes() {
    _drawer->release();
    SceneNode::releaseRes();
}

}
}
