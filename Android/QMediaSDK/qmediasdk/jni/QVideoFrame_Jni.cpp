//
// Created by spring on 2020/5/20.
//

#include "QVideoFrame_Jni.h"
#include "j4a_generate/j4a_generate.h"
#include "GraphicCore/opengl/Drawable2D.h"
#include "GraphicCore/opengl/shaders/ccShaders.h"

USING_GRAPHICCORE

JavaVideoFrameBuffer::JavaVideoFrameBuffer(jobject jframe){
    JNIEnv* env = JniUtils::getEnv();
    _object = env->NewGlobalRef(jframe);
    bool isTexture = J4AC_com_qmedia_qmediasdk_QGraphic_QVideoFrame__is_texture(env, jframe);
    if (isTexture) {
        pixel_format_ = RawVideoFormat::kHardware;
        _pbuffer = NULL;
    }
    else {
        pixel_format_ = RawVideoFormat::kI420;
        jobject jbuffer = J4AC_com_qmedia_qmediasdk_QGraphic_QVideoFrame__buffer(env, jframe);
        _pbuffer = (uint8_t*)env->GetDirectBufferAddress(jbuffer);
    }
    _width = J4AC_com_qmedia_qmediasdk_QGraphic_QVideoFrame__width(env, _object);
    _height = J4AC_com_qmedia_qmediasdk_QGraphic_QVideoFrame__height(env, _object);
}
JavaVideoFrameBuffer::~JavaVideoFrameBuffer(){
    JNIEnv* env = JniUtils::getEnv();
    env->DeleteGlobalRef(_object);
}

int JavaVideoFrameBuffer::width() const {
    return _width;
}

int JavaVideoFrameBuffer::height() const {
    return _height;
}

const uint8_t *JavaVideoFrameBuffer::DataY() const {
    return _pbuffer;
}

const uint8_t *JavaVideoFrameBuffer::DataU() const {
    if (_pbuffer) {
        return _pbuffer + (_width * _height);
    }
    return nullptr;
}

const uint8_t *JavaVideoFrameBuffer::DataV() const {
    if (_pbuffer) {
        return _pbuffer + (_width * _height * 3 /2) ;
    }
    return nullptr;
}

int JavaVideoFrameBuffer::StrideY() const {
    return _width;
}

int JavaVideoFrameBuffer::StrideU() const {
    return _width / 2;
}

int JavaVideoFrameBuffer::StrideV() const {
    return _width / 2;
}

void *JavaVideoFrameBuffer::native_handle() const {
    JNIEnv* env = JniUtils::getEnv();
    return reinterpret_cast<void *>(J4AC_com_qmedia_qmediasdk_QGraphic_QVideoFrame__textureid(env, _object));
}

std::shared_ptr<VideoFrameBuffer> JavaVideoFrameBuffer::NativeToI420Buffer() {
    return nullptr;
}

VideoFrame createVideoFrameByObject(JNIEnv* env, jobject jframe) {
    std::shared_ptr<VideoFrameBuffer> jvideoframebuffer = std::shared_ptr<VideoFrameBuffer>(new JavaVideoFrameBuffer(jframe));
    jlong timestamp = J4AC_com_qmedia_qmediasdk_QGraphic_QVideoFrame__timestamp(env, jframe);
    return VideoFrame(jvideoframebuffer, timestamp);
}



const char* OSEPositionTexture_frag = R"(
#extension GL_OES_EGL_image_external : require
#ifdef GL_ES
varying mediump vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif

uniform samplerExternalOES uTexture;
uniform highp vec4 uColor;

void main()
{
    gl_FragColor = texture2D(uTexture, v_texCoord) * uColor;
}
)";

OESVideoFrameDrawer::OESVideoFrameDrawer(const VideoTarget *videoTarget) : VideoFrameDrawer(
        videoTarget), _rotation(kVideoRotation_0) {
    _textureid = 0;
}
bool OESVideoFrameDrawer::setFrame(const VideoFrame &videoFrame) {
    if (!videoFrame.is_texture()) {
        return false;
    }
    _texW = videoFrame.width();
    _texH = videoFrame.height();
    _rotation = videoFrame.rotation();
//    _textureid = static_cast<int>(reinterpret_cast<uintptr_t>(videoFrame.video_frame_buffer()->native_handle()));
    _textureid = (long)videoFrame.video_frame_buffer()->native_handle();
    if (!_program) {
        _program = std::shared_ptr<ShaderProgram>(new ShaderProgram());
        _program->createProgram(defaultPositionTexture_vert, OSEPositionTexture_frag);
    }
    return true;
}

void OESVideoFrameDrawer::drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* node)
{
    GraphicCore::Mat4 mvpMatrix;
    //FIXME: translation of position already contain in transform matrix
    GraphicCore::Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
    GraphicCore::Rect region(Vec2(0,0), node->getContentSize());
    drawFrame(mvpMatrix,region,node->getPositionZ(),node->getCrop(),node->getColor(),node->getBlendFunc(),_rotation);
}

void OESVideoFrameDrawer::drawFrame(const GraphicCore::Mat4& mvpMatrix, const GraphicCore::Rect & region, float positionZ, const GraphicCore::Rect crop, GraphicCore::Color4F color,
                                    const GraphicCore::BlendFunc& blend, VideoRotation rotation, GraphicCore::Drawable2D::FlipMode flipMode)
{
    float *texArray = Drawable2D::RECTANGLE_TEX_COORDS;
    switch (rotation) {
        case kVideoRotation_90:
            texArray = Drawable2D::RECTANGLE_TEX_COORDS90;
            break;
        case kVideoRotation_180:
            texArray = Drawable2D::RECTANGLE_TEX_COORDS180;
            break;
        case kVideoRotation_270:
            texArray = Drawable2D::RECTANGLE_TEX_COORDS270;
            break;
        default:
            texArray = Drawable2D::RECTANGLE_TEX_COORDS;
            break;

    }
    if (_program && _program->use()) {
        GLfloat posArray[] = {
                region.getMinX(), region.getMinY(), positionZ,
                region.getMaxX(), region.getMinY(), positionZ,
                region.getMinX(), region.getMaxY(), positionZ,
                region.getMaxX(), region.getMaxY(), positionZ
        };

        //check blend func
        if (blend != BlendFunc::DISABLE) {
            _program->setBlendFunc(blend);
        } else if (! FLOAT_ISEQUAL(color.a, 1.0f)){
            _program->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
        } else
            _program->setBlendFunc(BlendFunc::DISABLE);

        _program->setVertexAttribValue("a_texCoord", VertexAttrib::VERTEX2, texArray, 8);
        _program->setVertexAttribValue("a_position", VertexAttrib::VERTEX3 ,posArray, GET_ARRAY_COUNT(posArray));
        Uniform::Value textureVal;
        textureVal._textureTarget = GL_TEXTURE_EXTERNAL_OES;
        textureVal._texture = _textureid;
        _program->setUniformValue("uTexture", Uniform::TEXTURE, textureVal);
        GraphicCore::Mat4 texMatrix;
        //TODO: check filp mode
        switch (flipMode) {
            case Drawable2D::FlipH:
                texMatrix = Drawable2D::MtxFlipH;
                break;
            case Drawable2D::FlipV:
                texMatrix = Drawable2D::MtxFlipV;
                break;
            default:
                break;
        }

        //TODO: check crop
        if (! crop.size.equals(Size::ZERO)) {
            //TODO: need crop
            float crop_arr[16] = {
                    crop.size.width, 0, 0, 0,
                    0, crop.size.height, 0, 0,
                    0, 0, 1, 0,
                    crop.getMinX(), crop.getMinY(), 0, 1,
            };
            texMatrix.multiply(crop_arr);
        }
        _program->setUniformValue("uTexMatrix", Uniform::MATRIX4, texMatrix.m, 16);
        _program->setUniformValue("uMVPMatrix", Uniform::MATRIX4, (float*)mvpMatrix.m, 16);

        float colorArr[] = { color.r, color.g, color.b, color.a };
        _program->setUniformValue("uColor", Uniform::FLOAT4, colorArr, GET_ARRAY_COUNT(colorArr));

        _program->drawRectangle();
    }
}

void OESVideoFrameDrawer::release()
{
    if (_program) {
        _program->releaseProgram();
        _program.reset();
    }
}
