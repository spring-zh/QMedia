//
//  MediaGraphicChannel.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaGraphicChannel.h"
#include "MediaTrack.h"

MediaGraphicChannel::MediaGraphicChannel(MediaTrack* mediaTrack):_mediaTrack(mediaTrack), _lastUpdateTime(-1)
{
    _drawer = std::unique_ptr<VideoFrameDrawer>(mediaTrack->createVideoFrameDrawer());
}
MediaGraphicChannel::~MediaGraphicChannel()
{
    _drawer.reset();
}

void MediaGraphicChannel::draw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, uint32_t flags)
{
    updateFrame(scene->getDelta());
    
    int64_t duration = scene->getDelta() - _renderRange._start;
    if (_effect_group.has_effect() && _drawer->getOutputTexture()) {
        const GraphicCore::Texture2D* texture = _drawer->getOutputTexture();
        RenderNode::drawEffects(duration, scene, transform, texture);
    }else {
        _drawer->drawFrame(scene,transform, this);
    }
}

void MediaGraphicChannel::duplicateDraw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* diaplayNode)
{
    updateFrame(scene->getDelta());
    
    _drawer->drawFrame(scene,transform, diaplayNode);
}

void MediaGraphicChannel::updateFrame(int64_t time_ms) {
    if (_lastUpdateTime != time_ms) {
        _lastUpdateTime = time_ms;
        bool bReadEnd = false;
        VideoFrame video_frame = _mediaTrack->getVideoFrame(time_ms, bReadEnd);
        if (!bReadEnd && video_frame.video_frame_buffer()) {
            _drawer->setFrame(video_frame);
        }
    }
}

const Range<int64_t> MediaGraphicChannel::getRange()
{
    _renderRange = _mediaTrack->getDisplayTrackRange();
    return _renderRange;
}

void MediaGraphicChannel::releaseRes()
{
    _drawer->release();
    RenderNode::releaseRes();
}
