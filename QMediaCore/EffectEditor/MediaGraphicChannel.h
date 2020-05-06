//
//  MediaGraphicChannel.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_MEDIAGRAPHICCHANNEL_H
#define EFFECTEDITOR_MEDIAGRAPHICCHANNEL_H

#include "VideoFrameDrawer.h"
#include "GraphicCore/GcRenderNode.h"

class MediaTrack;

class MediaGraphicChannel : public GraphicCore::RenderNode {
public:
    MediaGraphicChannel(MediaTrack *mediaTrack);
    virtual ~MediaGraphicChannel();
    
    virtual void draw(GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, uint32_t /*flags*/) override;
    
    virtual void duplicateDraw(GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, const GraphicCore::Node* /*diaplayNode*/) override;
    
    virtual const Range<int64_t> getRange() override;
    
    void releaseRes() override;
    
private:
    
    MediaTrack *_mediaTrack;
    std::unique_ptr<VideoFrameDrawer> _drawer;
};

using MediaGraphicChannelRef = std::shared_ptr<MediaGraphicChannel>;

#endif /* EFFECTEDITOR_MEDIAGRAPHICCHANNEL_H */
