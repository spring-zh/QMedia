//
//  VideoRenderNodeImpl.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef QMEDIA_API_VIDEO_RENDER_NODE_IMPL_H
#define QMEDIA_API_VIDEO_RENDER_NODE_IMPL_H

#include "media_range.h"
#include "animator.h"
#include "video_render_node.h"
#include "Utils/Comm.h"
#include "MediaCore/core/DecodedFrame.h"

#include "size.h"
#include "point.h"
#include "vec4f.h"

#include "VideoFrameDrawer.h"
#include "GraphicCore/GcRenderNode.h"
#include "GraphicCore/GcScene.h"

namespace QMedia { namespace Api {

class MediaSegmentImpl;

class VideoRenderNodeImpl : public VideoRenderNode {
public:
    VideoRenderNodeImpl(MediaSegmentImpl* segment);
    ~VideoRenderNodeImpl();
    
    virtual void setVisible(bool visible) override;

    virtual void setAlpha(float alpha)  override;

    virtual void setCrop(const Vec4f & crop) override;

    virtual void setContentSize(const Size & content_size) override;

    virtual void setAnchorPoint(const Point & anchor_point) override;

    virtual void setPosition(const Point & position) override;

    virtual void setPositionZ(int32_t positionz) override;

    virtual void setRotation(int32_t rotation) override;

    virtual void setRotation3d(const Vec4f & rotation3d) override;

    virtual void setScaleX(float scalex) override;

    virtual void setScaleY(float scaley) override;

    virtual void setScaleZ(float scalez) override;

    virtual std::vector<Animator> getAnimators() override;

    virtual bool addAnimator(const Animator & animator) override;

    virtual bool removeAnimator(const Animator & animator) override;
    
    class MediaGraphicNode : public GraphicCore::RenderNode {
    public:
        MediaGraphicNode(VideoRenderNodeImpl* impl):impl_(impl), _lastUpdateTime(-1) {
            _drawer = std::unique_ptr<VideoFrameDrawer>(VideoFrameDrawer::createVideoFrameDrawer(RawVideoFormat::kHardware));
        }
        virtual ~MediaGraphicNode() {
            _drawer.reset();
        }
        
        void updateFrame(const VideoFrame& videoFrame) {
            _drawer->setFrame(videoFrame);
        }
        
        virtual void draw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, uint32_t flags) override {
            
            int64_t duration = scene->getDelta() - _renderRange._start;
            if (_effect_group.has_effect() && _drawer->getOutputTexture()) {
                const GraphicCore::Texture2D* texture = _drawer->getOutputTexture();
                RenderNode::drawEffects(duration, scene, transform, texture);
            }else {
                _drawer->drawFrame(scene,transform, this);
            }
        }
        
        virtual void duplicateDraw(GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* diaplayNode) override {
            _drawer->drawFrame(scene,transform, diaplayNode);
        }
        
        virtual const Range<int64_t> getRange() override {
            auto range = impl_->getRange();
            return Range<int64_t>(range.start, range.end);
        }
        
        void releaseRes() override {
            _drawer->release();
            RenderNode::releaseRes();
        }
        
    private:
        VideoRenderNodeImpl* impl_;
        int64_t _lastUpdateTime;
        std::unique_ptr<VideoFrameDrawer> _drawer;
    };
    
    // other
    const MediaRange getRange() const;
    MediaGraphicNode* getMediaGraphicNode() const;
    void PutVideoFrame(DecodedFrame videoframe);
private:
    MediaSegmentImpl* segment_;
    std::unique_ptr<MediaGraphicNode> graphic_;
};

}
}

#endif //QMEDIA_API_VIDEO_RENDER_NODE_IMPL_H
