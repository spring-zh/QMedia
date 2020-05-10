//
//  MediaGraphicChannel.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "MediaGraphicChannel.h"
#include "MediaTrack.h"

MediaGraphicChannel::MediaGraphicChannel(MediaTrack* mediaTrack):_mediaTrack(mediaTrack)
{
    _drawer = std::unique_ptr<VideoFrameDrawer>(mediaTrack->createVideoFrameDrawer());
}
MediaGraphicChannel::~MediaGraphicChannel()
{
    _drawer.reset();
}

void MediaGraphicChannel::draw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, uint32_t flags)
{
    duplicateDraw(scene, transform, this);
}

void MediaGraphicChannel::duplicateDraw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* diaplayNode)
{
    bool bReadEnd = false;
    VideoFrame video_frame = _mediaTrack->getVideoFrame(scene->getDelta(), bReadEnd);
    if (!bReadEnd && video_frame.video_frame_buffer()) {
        _drawer->setFrame(video_frame);
    }
    GraphicCore::Rect region(diaplayNode->getPosition(),diaplayNode->getContentSize());
    _drawer->drawFrame(scene,transform,region);
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
