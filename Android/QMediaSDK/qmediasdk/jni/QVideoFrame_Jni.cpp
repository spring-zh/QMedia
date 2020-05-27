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
    _object = env->NewWeakGlobalRef(jframe);
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
    env->DeleteWeakGlobalRef(_object);
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
    return (void *)J4AC_com_qmedia_qmediasdk_QGraphic_QVideoFrame__textureid(env, _object);
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
        videoTarget) {
    _textureid = 0;
}
bool OESVideoFrameDrawer::setFrame(const VideoFrame &videoFrame) {
    if (!videoFrame.is_texture()) {
        return false;
    }
    _texW = videoFrame.width();
    _texH = videoFrame.height();
//    _textureid = static_cast<int>(reinterpret_cast<uintptr_t>(videoFrame.video_frame_buffer()->native_handle()));
    _textureid = (long)videoFrame.video_frame_buffer()->native_handle();
    if (!_program) {
        _program = std::shared_ptr<ShaderProgram>(new ShaderProgram());
        if(_program->createProgram(defaultPositionTexture_vert, OSEPositionTexture_frag)){
            if(_program->use()){
                _program->addVertexAttribOption("a_position",VertexAttrib::VERTEX3);
                _program->addVertexAttribOption("a_texCoord",VertexAttrib::TEXCOORD);
                _program->addUniformOption("uMVPMatrix",Uniform::MATRIX4);
                _program->addUniformOption("uTexMatrix",Uniform::MATRIX4);
                _program->addUniformOption("uTexture",Uniform::TEXTURE);
                _program->addUniformOption("uColor",Uniform::FLOAT4);

            }
        }
    }
    return true;
}

void OESVideoFrameDrawer::drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* node)
{
    if (_program && _program->use()) {
        //FIXME: translation of position already contain in transform matrix
        GLfloat posArray[] = {
                0, 0, 0,
                node->getContentSize().width, 0, 0,
                0, node->getContentSize().height, 0,
                node->getContentSize().width, node->getContentSize().height, 0
        };

        _program->setVertexAttribValue("a_texCoord", GET_ARRAY_COUNT(Drawable2D::RECTANGLE_TEX_COORDS), Drawable2D::RECTANGLE_TEX_COORDS);
        _program->setVertexAttribValue("a_position", GET_ARRAY_COUNT(posArray) ,posArray);
        _program->setUniformValue("uTexture",_textureid);
        GraphicCore::Mat4 mvpMatrix;
        GraphicCore::Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
        GraphicCore::Mat4 texMatrix;
        _program->setUniformValue("uTexMatrix", 16, texMatrix.m/*Drawable2D::MtxFlipV*/);
        _program->setUniformValue("uMVPMatrix", 16, mvpMatrix.m);

        float colorArr[] = { node->getColor().r, node->getColor().g, node->getColor().b, node->getColor().a };
        _program->setUniformValue("uColor", GET_ARRAY_COUNT(colorArr), colorArr);
        if (! FLOAT_ISEQUAL(node->getColor().a, 1.0f)) {
            _program->enableBlend(true);
        }
        else
            _program->enableBlend(false);

        _program->drawRect();
    }
}
void OESVideoFrameDrawer::release()
{
    if (_program) {
        _program->releaseProgram();
        _program.reset();
    }
}
