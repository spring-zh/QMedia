//
//  VideoGraphicNode.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef EFFECTEDITOR_VIDEOGRAPHIC_NODE_H
#define EFFECTEDITOR_VIDEOGRAPHIC_NODE_H

#include "MediaCore/core/DecodedFrame.h"
#include "EffectEditor/VideoFrameDrawer.h"
#include "EffectEditor/MediaSegmentImpl.h"
#include "GraphicCore/SceneNode.h"

namespace QMedia { namespace Api {

class VideoGraphicNode : public SceneNode {
public:
    VideoGraphicNode(MediaSegmentImpl* segment);
    virtual ~VideoGraphicNode();
    
    void PutVideoFrame(DecodedFrame decodeframe);
    
    virtual void draw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, uint32_t flags) override ;
    
    virtual void duplicateDraw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, SceneNode* diaplayNode);
    
    virtual const Range<int64_t> getRange() override ;
    
    void releaseRes() override ;
    
private:
    MediaSegmentImpl* segment_;
    int64_t _lastUpdateTime;
    std::unique_ptr<VideoFrameDrawer> _drawer;
};

}
}

#endif /* EFFECTEDITOR_VIDEOGRAPHIC_NODE_H */
