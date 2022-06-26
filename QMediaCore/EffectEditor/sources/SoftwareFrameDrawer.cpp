//
// Created by spring on 2020/5/20.
//

#include "SoftwareFrameDrawer.h"
#include "RenderEngine/opengl/Drawable2D.h"
#include "RenderEngine/opengl/shader_code.h"

using namespace QMedia::RenderEngine;

const char* YUVPositionTexture_frag = R"(
#ifdef GL_ES
precision highp float;
#endif
varying vec2 v_texCoord;

uniform sampler2D SamplerY;
uniform sampler2D SamplerU;
uniform sampler2D SamplerV;

uniform vec4 uColor;

void main(void)
{
    vec3 rgb;
    vec3 yuv = vec3(texture2D(SamplerY, v_texCoord).r ,
                    texture2D(SamplerU, v_texCoord).r - 0.5,
                    texture2D(SamplerV, v_texCoord).r - 0.5);
	rgb = mat3( 1,       1,         1,
	            0,       -0.39465,  2.03211,
	            1.13983, -0.58060,  0) * yuv;
	gl_FragColor = vec4(rgb, 1) * uColor;
}
)";

YUVFrameDrawer::YUVFrameDrawer(const VideoTarget *videoTarget) : VideoFrameDrawer(videoTarget),
_textureY(0), _textureU(0), _textureV(0),
_texWidth(-1), _texHeight(-1), _rotation(kVideoRotation_0)
{
}
bool YUVFrameDrawer::setFrame(const VideoFrame &videoFrame) {
    if (videoFrame.get_video_format() != RawVideoFormat::kI420 && videoFrame.get_video_format() != RawVideoFormat::kYV12) {
        return false;
    }
    _rotation = videoFrame.rotation();
    if (_texWidth != videoFrame.width() || _texHeight != videoFrame.height()) {
        //TODO: video size change

        if (_textureY) {
            glDeleteTextures(1, (const GLuint *)(&_textureY));
        }
        if (_textureU) {
            glDeleteTextures(1, (const GLuint *)(&_textureU));
        }
        if (_textureV) {
            glDeleteTextures(1, (const GLuint *)(&_textureV));
        }

        _texWidth = videoFrame.width();
        _texHeight = videoFrame.height();

        int strideY = videoFrame.video_frame_buffer()->StrideY();
        int strideUV = videoFrame.video_frame_buffer()->StrideU();

        glGenTextures(1, &_textureY);
        glBindTexture(GL_TEXTURE_2D, _textureY);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, strideY, _texHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoFrame.video_frame_buffer()->DataY());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenTextures(1, &_textureU);
        glBindTexture(GL_TEXTURE_2D, _textureU);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, strideUV, _texHeight/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoFrame.video_frame_buffer()->DataU());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenTextures(1, &_textureV);
        glBindTexture(GL_TEXTURE_2D, _textureV);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, strideUV, _texHeight/2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoFrame.video_frame_buffer()->DataV());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        int strideY = videoFrame.video_frame_buffer()->StrideY();
        int strideUV = videoFrame.video_frame_buffer()->StrideU();
        glBindTexture(GL_TEXTURE_2D, _textureY);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, strideY, _texHeight, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoFrame.video_frame_buffer()->DataY());
        glBindTexture(GL_TEXTURE_2D, _textureU);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, strideUV, _texHeight/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoFrame.video_frame_buffer()->DataU());
        glBindTexture(GL_TEXTURE_2D, _textureV);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, strideUV, _texHeight/2, GL_LUMINANCE, GL_UNSIGNED_BYTE, videoFrame.video_frame_buffer()->DataV());
    }

    if (!_program) {
        _program = std::shared_ptr<ShaderProgram>(new ShaderProgram());
        _program->createProgram(defaultPositionTexture_vert, YUVPositionTexture_frag);
    }
    return true;
}

//void YUVFrameDrawer::drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* node)
//{
//    GraphicCore::Mat4 mvpMatrix;
//    //FIXME: translation of position already contain in transform matrix
//    GraphicCore::Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
//    GraphicCore::Rect region(Vec2(0,0), node->getContentSize());
//    drawFrame(mvpMatrix,region,node->getPositionZ(),node->getCrop(),node->getColor(),node->getBlendFunc(),_rotation);
//}

void YUVFrameDrawer::drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & region, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
                               const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode)
{
    if (_program && _program->use()) {
        VertexAttrib::Vec3<float> pos_vertex[] = {
            {region.getMinX(), region.getMinY(), positionZ},
            {region.getMaxX(), region.getMinY(), positionZ},
            {region.getMinX(), region.getMaxY(), positionZ},
            {region.getMaxX(), region.getMaxY(), positionZ}
        };
//        GLfloat posArray[] = {
//                region.getMinX(), region.getMinY(), positionZ,
//                region.getMaxX(), region.getMinY(), positionZ,
//                region.getMinX(), region.getMaxY(), positionZ,
//                region.getMaxX(), region.getMaxY(), positionZ
//        };
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

        //check blend func
        if (blend != BlendFunc::DISABLE) {
            _program->setBlendFunc(blend);
        } else if (! FLOAT_ISEQUAL(color.a, 1.0f)){
            _program->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
        } else
            _program->setBlendFunc(BlendFunc::DISABLE);

        _program->setVertexAttribValue("a_texCoord", (VertexAttrib::Vec2<float>*)(texArray), 4);
        _program->setVertexAttribValue("a_position", pos_vertex, 4);
        Uniform::TextureUnit textureYVal(_textureY);
        _program->setUniformValue("SamplerY", textureYVal);
        Uniform::TextureUnit textureUVal(_textureU);
        _program->setUniformValue("SamplerU", textureUVal);
        Uniform::TextureUnit textureVVal(_textureV);
        _program->setUniformValue("SamplerV", textureVVal);

        RenderEngine::Mat4 texMatrix;
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
        _program->setUniformValue("uTexMatrix", Uniform::Matrix(texMatrix.m, 4, 1));
        _program->setUniformValue("uMVPMatrix", Uniform::Matrix(mvpMatrix.m, 4, 1));

        Uniform::Vec4<float> color4 = { color.r, color.g, color.b, color.a };
        _program->setUniformValue("uColor", color4);

        _program->drawRectangle();
    }
}

void YUVFrameDrawer::release()
{
    if (_program) {
        _program->releaseProgram();
        _program.reset();
    }
}

#pragma mark ---------  RGBAFrameDrawer  ----------------
RGBAFrameDrawer::RGBAFrameDrawer(const VideoTarget *videoTarget):
VideoFrameDrawer(videoTarget),
_texture(0), _texWidth(-1), _texHeight(-1){

}
RGBAFrameDrawer::~RGBAFrameDrawer() {

}

bool RGBAFrameDrawer::setFrame(const VideoFrame& videoFrame) {
    if (videoFrame.get_video_format() != RawVideoFormat::kARGB && videoFrame.get_video_format() != RawVideoFormat::kBGRA) {
        return false;
    }

    if (_texWidth != videoFrame.width() || _texHeight != videoFrame.height()) {
        //TODO: video size change
        _texWidth = videoFrame.width();
        _texHeight = videoFrame.height();
        int strideY = videoFrame.video_frame_buffer()->StrideY();
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, strideY/4, _texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, videoFrame.video_frame_buffer()->DataY());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        int strideY = videoFrame.video_frame_buffer()->StrideY();
        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, strideY/4, _texHeight, GL_RGBA, GL_UNSIGNED_BYTE, videoFrame.video_frame_buffer()->DataY());
    }

    _duplicateTexture.updateTexture(_texture, Texture2D::RGBA, _texWidth, _texHeight);
    if (!_textureDrawer) {
        _textureDrawer = std::shared_ptr<RenderEngine::Texture2DDrawer>(new RenderEngine::Texture2DDrawer());
    }

    return true;
}
//void RGBAFrameDrawer::drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* node) {
//    if (_textureDrawer) {
//        _textureDrawer->draw(&_duplicateTexture, scene, transform, node);
//    }
//}
void RGBAFrameDrawer::drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & region, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
                                     const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode) {
    if (_textureDrawer) {
        _textureDrawer->draw(&_duplicateTexture, mvpMatrix, region, positionZ, crop, color, blend, flipMode);
    }
}

void RGBAFrameDrawer::release() {
    if (_textureDrawer) {
        _textureDrawer.reset();
    }
    if (_texture) {
        glDeleteTextures(1, &_texture);
        _texture = 0;
    }
}
