//
// Created by spring on 2020/5/20.
//

#ifndef QMEDIASDK_QVIDEOFRAME_JNI_H
#define QMEDIASDK_QVIDEOFRAME_JNI_H

#include "JniUtils.h"
#include "GraphicCore/opengl/ShaderProgram.h"
#include "MediaCore/core/VideoFrame.h"
#include "EffectEditor/VideoFrameDrawer.h"

using GraphicCore::ShaderProgram;

class JavaVideoFrameBuffer : public VideoFrameBuffer {
public:
    JavaVideoFrameBuffer(jobject jframe);
    ~JavaVideoFrameBuffer();
    int width() const override;

    int height() const override;

    const uint8_t *DataY() const override;

    const uint8_t *DataU() const override;

    const uint8_t *DataV() const override;

    int StrideY() const override;

    int StrideU() const override;

    int StrideV() const override;

    void *native_handle() const override;

    std::shared_ptr<VideoFrameBuffer> NativeToI420Buffer() override;

protected:
    jobject _object;
    uint8_t *_pbuffer;
    int _width;
    int _height;
};

extern VideoFrame createVideoFrameByObject(JNIEnv* env,jobject jframe);

class OESVideoFrameDrawer : public VideoFrameDrawer {
public:
    explicit OESVideoFrameDrawer(const VideoTarget *videoTarget);
    ~OESVideoFrameDrawer() = default;

    bool setFrame(const VideoFrame &videoFrame) override;
    virtual void drawFrame(const GraphicCore::Scene* /*scene*/, const GraphicCore::Mat4 & /*transform*/, const GraphicCore::Node* node) override ;
    virtual void drawFrameDirect(const GraphicCore::Scene* /*scene*/, const GraphicCore::Rect & /*region*/, const GraphicCore::Rect crop, GraphicCore::Color4F color, GraphicCore::Drawable2D::FlipMode flipMode) override ;
    virtual void release() override;

protected:
    std::shared_ptr<ShaderProgram> _program;
    int _textureid;
    int _texW;
    int _texH;
};

#endif //QMEDIASDK_QVIDEOFRAME_JNI_H
