//
// Created by spring on 2020/5/20.
//

#include "SoftwareFrameDrawer.h"
#include "GraphicCore/opengl/Drawable2D.h"
#include "GraphicCore/opengl/shaders/ccShaders.h"

USING_GRAPHICCORE

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
_texWidth(-1), _texHeight(-1) {
}
bool YUVFrameDrawer::setFrame(const VideoFrame &videoFrame) {
    if (videoFrame.get_video_format() != RawVideoFormat::kI420 && videoFrame.get_video_format() != RawVideoFormat::kYV12) {
        return false;
    }
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

void YUVFrameDrawer::drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* node)
{
    if (_program && _program->use()) {
        //FIXME: translation of position already contain in transform matrix
        GLfloat posArray[] = {
                0, 0, 0,
                node->getContentSize().width, 0, 0,
                0, node->getContentSize().height, 0,
                node->getContentSize().width, node->getContentSize().height, 0
        };

        _program->setVertexAttribValue("a_texCoord", VertexAttrib::VERTEX2, Drawable2D::RECTANGLE_TEX_COORDS, GET_ARRAY_COUNT(Drawable2D::RECTANGLE_TEX_COORDS));
        _program->setVertexAttribValue("a_position", VertexAttrib::VERTEX3 ,posArray, GET_ARRAY_COUNT(posArray));
        Uniform::Value textureYVal;
        textureYVal._texture = _textureY;
        _program->setUniformValue("SamplerY", Uniform::TEXTURE, textureYVal);
        Uniform::Value textureUVal;
        textureUVal._texture = _textureU;
        _program->setUniformValue("SamplerU", Uniform::TEXTURE, textureUVal);
        Uniform::Value textureVVal;
        textureVVal._texture = _textureV;
        _program->setUniformValue("SamplerV", Uniform::TEXTURE, textureVVal);
        GraphicCore::Mat4 mvpMatrix;
        GraphicCore::Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
        GraphicCore::Mat4 texMatrix;
        _program->setUniformValue("uTexMatrix", Uniform::MATRIX4, texMatrix.m/*Drawable2D::MtxFlipV*/, 16);
        _program->setUniformValue("uMVPMatrix", Uniform::MATRIX4, mvpMatrix.m, 16);

        float colorArr[] = { node->getColor().r, node->getColor().g, node->getColor().b, node->getColor().a };
        _program->setUniformValue("uColor", Uniform::FLOAT4, colorArr, GET_ARRAY_COUNT(colorArr));
        if (node->getBlendFunc() != BlendFunc::DISABLE) {
            _program->setBlendFunc(node->getBlendFunc());
        } else if (! FLOAT_ISEQUAL(node->getColor().a, 1.0f)){
            _program->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
        } else
            _program->setBlendFunc(BlendFunc::DISABLE);

        _program->drawRectangle();
    }
}

void YUVFrameDrawer::drawFrameDirect(const GraphicCore::Scene* scene, const GraphicCore::Rect & region, const GraphicCore::Rect crop, GraphicCore::Color4F color, GraphicCore::Drawable2D::FlipMode flipMode)
{
    if (_program && _program->use()) {
        GLfloat posArray[] = {
                region.getMinX(), region.getMinY(), 0,
                region.getMaxX(), region.getMinY(), 0,
                region.getMinX(), region.getMaxY(), 0,
                region.getMaxX(), region.getMaxY(), 0
        };

        _program->setVertexAttribValue("a_texCoord", VertexAttrib::VERTEX2, Drawable2D::RECTANGLE_TEX_COORDS, GET_ARRAY_COUNT(Drawable2D::RECTANGLE_TEX_COORDS));
        _program->setVertexAttribValue("a_position", VertexAttrib::VERTEX3 ,posArray, GET_ARRAY_COUNT(posArray));
        Uniform::Value textureYVal;
        textureYVal._texture = _textureY;
        _program->setUniformValue("SamplerY", Uniform::TEXTURE, textureYVal);
        Uniform::Value textureUVal;
        textureUVal._texture = _textureY;
        _program->setUniformValue("SamplerU", Uniform::TEXTURE, textureUVal);
        Uniform::Value textureVVal;
        textureVVal._texture = _textureY;
        _program->setUniformValue("SamplerV", Uniform::TEXTURE, textureVVal);
        GraphicCore::Mat4 mvpMatrix = scene->getMatrix(MATRIX_STACK_PROJECTION);
//        GraphicCore::Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
        GraphicCore::Mat4 texMatrix;
        _program->setUniformValue("uTexMatrix", Uniform::MATRIX4, texMatrix.m/*Drawable2D::MtxFlipV*/, 16);
        _program->setUniformValue("uMVPMatrix", Uniform::MATRIX4, mvpMatrix.m, 16);

        float colorArr[] = { color.r, color.g, color.b, color.a };
        _program->setUniformValue("uColor", Uniform::FLOAT4, colorArr, GET_ARRAY_COUNT(colorArr));
        if (! FLOAT_ISEQUAL(color.a, 1.0f)){
            _program->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
        } else
            _program->setBlendFunc(BlendFunc::DISABLE);

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
        _textureDrawer = std::shared_ptr<GraphicCore::Texture2DDrawer>(new GraphicCore::Texture2DDrawer());
    }

    return true;
}
void RGBAFrameDrawer::drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* node) {
    if (_textureDrawer) {
        _textureDrawer->draw(&_duplicateTexture, scene, transform, node);
    }
}
void RGBAFrameDrawer::drawFrameDirect(const GraphicCore::Scene* scene, const GraphicCore::Rect & region, const GraphicCore::Rect crop, GraphicCore::Color4F color, GraphicCore::Drawable2D::FlipMode flipMode) {
    if (_textureDrawer) {
        _textureDrawer->drawDirect(&_duplicateTexture, scene, region, crop, color, flipMode);
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