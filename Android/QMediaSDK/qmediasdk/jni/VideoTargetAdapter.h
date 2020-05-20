//
// Created by spring on 2020/5/19.
//

#ifndef QMEDIASDK_VIDEOTARGETADAPTER_H
#define QMEDIASDK_VIDEOTARGETADAPTER_H

#include "JniUtils.h"
#include "MediaCore/output/VideoTarget.h"

class VideoTargetAdapter : public  VideoTarget{
public:
    VideoTargetAdapter(jobject jtarget) ;
    ~VideoTargetAdapter() ;
    bool start() override;

    bool stop() override;

    void pause(bool isPause) override;

    void flush() override;

    bool init(VideoDescribe videoDesc) override;

    const int getWidth() const override;

    const int getHeight() const override;

    const float getFrameRate() const override;

    void * getRenderContext() const override;

protected:

    jobject _object;
};


#endif //QMEDIASDK_VIDEOTARGETADAPTER_H
