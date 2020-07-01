//
//  QVideoTarget_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QVideoTarget.h"
#include "MediaCore/output/VideoTarget.h"

extern QVideoDescribe* videoDescribeToXM(struct VideoDescribe desc);

#pragma mark XMVideoTarget(object-c) to VideoTarget(c++)
class VideoTargetAdapter: public VideoTarget {
public:
    explicit VideoTargetAdapter(__weak id<QVideoTarget> ocTarget):_ocTarget(ocTarget) {}
    ~VideoTargetAdapter() {}
    
    virtual bool start() override {
        return [_ocTarget startVideo];
    }
    virtual bool stop() override {
        return [_ocTarget stopVideo];
    }
    virtual void pause(bool isPause) override {
        return [_ocTarget pauseVideo:isPause];
    }
    virtual void flush() override {
        return [_ocTarget flushVideo];
    }
    
    //for video target
    virtual bool init(VideoDescribe videoDesc) override {
        QVideoDescribe *xmVideoDesc = videoDescribeToXM(videoDesc);
        return [_ocTarget initVideo:xmVideoDesc];
    }
    virtual void * getRenderContext() const override {
        return (__bridge void *)[_ocTarget getContext];
    }
    const int getWidth() const override { return [_ocTarget getWidth];}
    const int getHeight() const override { return [_ocTarget getHeight];}
    const float getFrameRate() const override {return [_ocTarget getFrameRate];}
    
    //wrapper video render interface
    bool onVideoRender(int64_t wantTime) { return _render->onVideoRender(wantTime); }
    bool onRenderCreate() { return _render->onRenderCreate(); }
    void onRenderDestroy() { _render->onRenderDestroy(); }
    
    __weak id<QVideoTarget> _ocTarget;
};
