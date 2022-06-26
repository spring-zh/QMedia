
#include "QVideoFrame.h"
#include "Utils/Comm.h"
#include "RenderEngine/opengl/Drawable2D.h"
#include "RenderEngine/opengl/shader_code.h"
#import <OpenGLES/EAGL.h>
#import <CoreFoundation/CoreFoundation.h>

using namespace QMedia::RenderEngine;

PixelFrameBuffer::PixelFrameBuffer(CMSampleBufferRef sampleBuffer){
//    _sampleBuffer = sampleBuffer;
//    CFRetain(_sampleBuffer);
    _imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    CFRetain(_imageBuffer);
    pixel_format_ = RawVideoFormat::kHardware;
    //get image info
    _width = (int) CVPixelBufferGetHeight(_imageBuffer);
    _heigth = (int) CVPixelBufferGetWidth(_imageBuffer);
    _data_size = (int)CVPixelBufferGetDataSize(_imageBuffer);
    _formatType = CVPixelBufferGetPixelFormatType(_imageBuffer);
    
    //get timestamp
    CMTime currentSampleTime = CMSampleBufferGetOutputPresentationTimeStamp(sampleBuffer);
    _time_stamp = CMTimeGetSeconds(currentSampleTime) * kNumMillisecsPerSec;
}
PixelFrameBuffer::~PixelFrameBuffer(){
//    CMSampleBufferInvalidate(_sampleBuffer);
//    CFRelease(_sampleBuffer);
    CFRelease(_imageBuffer);
}

int PixelFrameBuffer::width() const {
    return _width;
}
int PixelFrameBuffer::height() const {
    return _heigth;
}

const uint8_t* PixelFrameBuffer::DataY() const {
    return (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_imageBuffer,0);
}
const uint8_t* PixelFrameBuffer::DataU() const {
    if (CVPixelBufferGetPlaneCount(_imageBuffer) >= 2) {
        return (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_imageBuffer,1);
    }else
        return NULL;
}
const uint8_t* PixelFrameBuffer::DataV() const {
    if (CVPixelBufferGetPlaneCount(_imageBuffer) >= 3) {
        return (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(_imageBuffer,2);
    }else
        return NULL;
}

int PixelFrameBuffer::StrideY() const {
    return (int)CVPixelBufferGetBytesPerRowOfPlane(_imageBuffer,0);
}
int PixelFrameBuffer::StrideU() const {
    if (CVPixelBufferGetPlaneCount(_imageBuffer) >= 2) {
        return (int)CVPixelBufferGetBytesPerRowOfPlane(_imageBuffer,1);
    }else
        return 0;
}
int PixelFrameBuffer::StrideV() const {
    if (CVPixelBufferGetPlaneCount(_imageBuffer) >= 3) {
        return (int)CVPixelBufferGetBytesPerRowOfPlane(_imageBuffer,2);
    }else
        return 0;
}

void* PixelFrameBuffer::native_handle() const {
    return _imageBuffer;
}

std::shared_ptr<VideoFrameBuffer> PixelFrameBuffer::NativeToI420Buffer() {
    return nullptr;
}

#pragma mask PixelFrameDrawer

//const char* PositionTexture_vert = R"(
//precision mediump float;
//
//attribute vec4 a_position;
//attribute vec4 a_texCoord;
//uniform mat4 MVPMatrix;
//uniform mat4 TexMatrix;
//
//#ifdef GL_ES
//varying mediump vec2 v_texCoord;
//#else
//varying vec2 v_texCoord;
//#endif
//
//void main()
//{
//    gl_Position = MVPMatrix * a_position;
//    v_texCoord = (TexMatrix * a_texCoord).xy;
//}
//)";

static const char * PositionTextureNV12_frag = R"(
precision mediump float;
#ifdef GL_ES
varying mediump vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif
uniform sampler2D SamplerY;
uniform sampler2D SamplerUV;
uniform mat3 colorConversionMatrix;
uniform vec4 uColor;

void main()
{
    vec3 yuv;
     
    // Subtract constants to map the video range start at 0
    yuv.x = (texture2D(SamplerY, v_texCoord).a - (16.0 / 255.0));
    yuv.yz = (texture2D(SamplerUV, v_texCoord).ra - vec2(0.5, 0.5));
    vec3 rgb = colorConversionMatrix * yuv;
    gl_FragColor = vec4(rgb, uColor.a);
}
)";

// Color Conversion Constants (YUV to RGB) including adjustment from 16-235/16-240 (video range)

// BT.601, which is the standard for SDTV.
GLfloat kColorConversion601Default[] = {
    1.164,  1.164, 1.164,
    0.0, -0.392, 2.017,
    1.596, -0.813,   0.0,
};

// BT.601 full range (ref: http://www.equasys.de/colorconversion.html)
GLfloat kColorConversion601FullRangeDefault[] = {
    1.0,    1.0,    1.0,
    0.0,    -0.343, 1.765,
    1.4,    -0.711, 0.0,
};

// BT.709, which is the standard for HDTV.
GLfloat kColorConversion709Default[] = {
    1.164,  1.164, 1.164,
    0.0, -0.213, 2.112,
    1.793, -0.533,   0.0,
};


GLfloat *kColorConversion601 = kColorConversion601Default;
GLfloat *kColorConversion601FullRange = kColorConversion601FullRangeDefault;
GLfloat *kColorConversion709 = kColorConversion709Default;

#pragma mark PixelFrameNV12Drawer

PixelFrameNV12Drawer::PixelFrameNV12Drawer(const VideoTarget *videoTarget):
VideoFrameDrawer(videoTarget)
{
    _videoTextureCache = NULL;
    _lumaTexture = NULL;
    _chromaTexture = NULL;
    
    _textures[0] = 0;
    _textures[1] = 0;
    
    _colorConversionMatrix = kColorConversion601;
}

PixelFrameNV12Drawer::~PixelFrameNV12Drawer()
{
    //FIXME: clearTexture must be called on render thread
    clearTexture();
}

void PixelFrameNV12Drawer::clearTexture()
{
    if (_lumaTexture) {
        CFRelease(_lumaTexture);
        _lumaTexture = NULL;
    }
    if (_chromaTexture) {
        CFRelease(_chromaTexture);
        _chromaTexture = NULL;
    }
    if (_videoTextureCache) {
        CVOpenGLESTextureCacheFlush(_videoTextureCache, 0);
        _videoTextureCache = NULL;
    }
    
}

bool PixelFrameNV12Drawer::setFrame(const VideoFrame& videoFrame)
{
    if (!videoFrame.is_texture()) {
        return false;
    }
    _rotation = videoFrame.rotation();
    EAGLContext* context = [EAGLContext currentContext];
    if (context == nil) {
        return false;
    }
    CVPixelBufferRef pixelBuffer = (CVPixelBufferRef)videoFrame.video_frame_buffer()->native_handle();
    int planeCount = (int)CVPixelBufferGetPlaneCount(pixelBuffer);
    uint32_t frameWidth = (uint32_t)CVPixelBufferGetWidth(pixelBuffer);
    uint32_t frameHeight = (uint32_t)CVPixelBufferGetHeight(pixelBuffer);
    
    CVReturn err = 0;
    
    // Create CVOpenGLESTextureCacheRef for optimal CVPixelBufferRef to GLES texture conversion.
    if (_videoTextureCache == NULL) {
        err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, context, NULL, &_videoTextureCache);
        if (err != noErr) {
            NSLog(@"PixelFrameDrawer Error at CVOpenGLESTextureCacheCreate %d", err);
            return false;
        }
    }
    CVOpenGLESTextureCacheFlush(_videoTextureCache, 0);
    
    if (_lumaTexture) {
        CFRelease(_lumaTexture);
        _lumaTexture = NULL;
    }
    if (_chromaTexture) {
        CFRelease(_chromaTexture);
        _chromaTexture = NULL;
    }

    glActiveTexture(GL_TEXTURE0);
    
    err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                       _videoTextureCache,
                                                       pixelBuffer,
                                                       NULL,
                                                       GL_TEXTURE_2D,
                                                       GL_ALPHA,
                                                       frameWidth,
                                                       frameHeight,
                                                       GL_ALPHA,
                                                       GL_UNSIGNED_BYTE,
                                                       0,
                                                       &_lumaTexture);
    if (err) {
        NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        return false;
    }else {
        _textures[0] = CVOpenGLESTextureGetName(_lumaTexture);
        glBindTexture(CVOpenGLESTextureGetTarget(_lumaTexture), _textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if(planeCount == 2) {
        // UV-plane.
        glActiveTexture(GL_TEXTURE1);
        err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                           _videoTextureCache,
                                                           pixelBuffer,
                                                           NULL,
                                                           GL_TEXTURE_2D,
                                                           GL_LUMINANCE_ALPHA,
                                                           frameWidth / 2,
                                                           frameHeight / 2,
                                                           GL_LUMINANCE_ALPHA,
                                                           GL_UNSIGNED_BYTE,
                                                           1,
                                                           &_chromaTexture);
        if (err) {
            NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        }else {
            _textures[1] = CVOpenGLESTextureGetName(_chromaTexture);
            glBindTexture(CVOpenGLESTextureGetTarget(_chromaTexture), _textures[1]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
    }

    CFStringRef color_attachments = (CFStringRef)CVBufferGetAttachment(pixelBuffer, kCVImageBufferYCbCrMatrixKey, NULL);
    if (color_attachments != nil && CFStringCompare(color_attachments, kCVImageBufferYCbCrMatrix_ITU_R_601_4, 0) == kCFCompareEqualTo){
//        glUniformMatrix3fv(_uniform[0], 1, GL_FALSE, kColorConversion601);
        _colorConversionMatrix = kColorConversion601;
    }else {
//        glUniformMatrix3fv(_uniform[0], 1, GL_FALSE, kColorConversion709);
        _colorConversionMatrix = kColorConversion709;
    }
    
    if (!_program) {
        _program = std::shared_ptr<ShaderProgram>(new ShaderProgram());
        _program->createProgram(defaultPositionTexture_vert, PositionTextureNV12_frag);
    }
    
    return true;
}

void PixelFrameNV12Drawer::drawFrame(const RenderEngine::Scene* scene, const RenderEngine::Mat4 & transform, QMedia::Api::SceneNode* node) {
    RenderEngine::Mat4 mvpMatrix;
    RenderEngine::Mat4::multiply(scene->getMatrix(Api::MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
    RenderEngine::Rect region(0,0, node->getContentSize().width, node->getContentSize().height);
    auto crop_v = node->getCrop();
    RenderEngine::Rect crop(crop_v.left, crop_v.top, crop_v.right, crop_v.bottom);
    auto color_v = node->getColor4F();
    RenderEngine::Color4F color = RenderEngine::Color4F(color_v.r, color_v.g, color_v.b, color_v.a);
    RenderEngine::BlendFunc bf = BlendFunc::DISABLE;
    drawFrame(mvpMatrix, region, (float)node->getPositionZ(), crop, color, bf, _rotation);
}

//void PixelFrameNV12Drawer::drawFrame(const RenderEngine::Scene* scene, const RenderEngine::Mat4 & transform, const RenderEngine::Node* node)
//{
//    RenderEngine::Mat4 mvpMatrix;
//    RenderEngine::Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
//    RenderEngine::Rect region(Vec2(0,0), node->getContentSize());
//    drawFrame(mvpMatrix, region, node->getPositionZ(), node->getCrop(), node->getColor(), node->getBlendFunc(), _rotation);
//}

void PixelFrameNV12Drawer::drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & region, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode) {
    
    if (_program && _program->use()) {
        GLfloat posArray[] = {
            region.getMinX(), region.getMinY(), positionZ,
            region.getMaxX(), region.getMinY(), positionZ,
            region.getMinX(), region.getMaxY(), positionZ,
            region.getMaxX(), region.getMaxY(), positionZ
        };
        
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
        
        _program->setVertexAttribValue("a_texCoord", (Uniform::Vec2<float>*)texArray, 4);
        _program->setVertexAttribValue("a_position", (Uniform::Vec3<float>*)posArray, 4);
        
        if(_lumaTexture) {
            Uniform::TextureUnit texture_y(CVOpenGLESTextureGetName(_lumaTexture), CVOpenGLESTextureGetTarget(_lumaTexture));
            _program->setUniformValue("SamplerY", texture_y);
        }
        if (_chromaTexture) {
            Uniform::TextureUnit texture_uv(CVOpenGLESTextureGetName(_chromaTexture), CVOpenGLESTextureGetTarget(_chromaTexture));
            _program->setUniformValue("SamplerUV", texture_uv);
        }
        
        RenderEngine::Mat4 texMatrix;
        //TODO: check crop
        if (! crop.size.equals(RenderEngine::Size::ZERO)) {
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
        _program->setUniformValue("colorConversionMatrix", Uniform::Matrix(_colorConversionMatrix, 3, 1));
        
        _program->setUniformValue("uColor", Uniform::Vec4<float>(color.r,color.g,color.b,color.a));
        if (blend != BlendFunc::DISABLE) {
            _program->setBlendFunc(blend);
        } else if (! FLOAT_ISEQUAL(color.a, 1.0f)){
            _program->setBlendFunc(BlendFunc::ALPHA_NON_PREMULTIPLIED);
        } else
            _program->setBlendFunc(BlendFunc::DISABLE);
        
        _program->drawRectangle();
    }
}

void PixelFrameNV12Drawer::release()
{
    if (_program) {
        _program->releaseProgram();
        _program.reset();
    }
    clearTexture();
}


#pragma mark PixelFrameBGRADrawer

PixelFrameBGRADrawer::PixelFrameBGRADrawer(const VideoTarget *videoTarget):
VideoFrameDrawer(videoTarget) , _textureDrawer(nullptr)
{
    _videoTextureCache = NULL;
    _textureRef = NULL;
    
    _textures = 0;
    
    _colorConversionMatrix = kColorConversion601;
}

PixelFrameBGRADrawer::~PixelFrameBGRADrawer()
{
    //FIXME: clearTexture must be called on render thread
    clearTexture();
}

void PixelFrameBGRADrawer::clearTexture()
{
    if (_textureRef) {
        CFRelease(_textureRef);
        _textureRef = NULL;
    }
    if (_videoTextureCache) {
        CVOpenGLESTextureCacheFlush(_videoTextureCache, 0);
        _videoTextureCache = NULL;
    }
    
}

bool PixelFrameBGRADrawer::setFrame(const VideoFrame& videoFrame)
{
    if (!videoFrame.is_texture()) {
        return false;
    }
    EAGLContext* context = [EAGLContext currentContext];
    if (context == nil) {
        return false;
    }
    CVPixelBufferRef pixelBuffer = (CVPixelBufferRef)videoFrame.video_frame_buffer()->native_handle();
//    int planeCount = (int)CVPixelBufferGetPlaneCount(pixelBuffer);
    uint32_t frameWidth = (uint32_t)CVPixelBufferGetWidth(pixelBuffer);
    uint32_t frameHeight = (uint32_t)CVPixelBufferGetHeight(pixelBuffer);
    
    CVReturn err = 0;
    
    // Create CVOpenGLESTextureCacheRef for optimal CVPixelBufferRef to GLES texture conversion.
    if (_videoTextureCache == NULL) {
        err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, context, NULL, &_videoTextureCache);
        if (err != noErr) {
            NSLog(@"PixelFrameDrawer Error at CVOpenGLESTextureCacheCreate %d", err);
            return false;
        }
    }
    CVOpenGLESTextureCacheFlush(_videoTextureCache, 0);
    
    if (_textureRef) {
        CFRelease(_textureRef);
        _textureRef = NULL;
    }

    glActiveTexture(GL_TEXTURE0);
    
    err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _videoTextureCache,
                                                        pixelBuffer,
                                                        NULL, // texture attributes
                                                        GL_TEXTURE_2D ,
                                                        GL_RGBA , // opengl format
                                                        frameWidth,
                                                        frameHeight,
                                                        GL_BGRA , // native iOS format
                                                        GL_UNSIGNED_BYTE  ,
                                                        0,
                                                        &_textureRef);

    if (err) {
        NSLog(@"Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        return false;
    }else {
        _textures = CVOpenGLESTextureGetName(_textureRef);
        glBindTexture(CVOpenGLESTextureGetTarget(_textureRef), _textures);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    CFStringRef color_attachments = (CFStringRef)CVBufferGetAttachment(pixelBuffer, kCVImageBufferYCbCrMatrixKey, NULL);
    if (color_attachments != nil && CFStringCompare(color_attachments, kCVImageBufferYCbCrMatrix_ITU_R_601_4, 0) == kCFCompareEqualTo){
        _colorConversionMatrix = kColorConversion601;
    }else {
        _colorConversionMatrix = kColorConversion709;
    }
    
    _duplicateTexture.updateTexture(_textures, Texture2D::RGBA, frameWidth, frameHeight);
    if (!_textureDrawer) {
        _textureDrawer = std::shared_ptr<RenderEngine::Texture2DDrawer>(new RenderEngine::Texture2DDrawer());
    }
    
    return true;
}

//void PixelFrameBGRADrawer::drawFrame(const RenderEngine::Scene* scene, const RenderEngine::Mat4 & transform, const RenderEngine::Node* node)
//{
//    if (_textureDrawer) {
//        _textureDrawer->draw(&_duplicateTexture, scene, transform, node);
//    }
//}

void PixelFrameBGRADrawer::drawFrame(const RenderEngine::Mat4& mvpMatrix, const RenderEngine::Rect & region, float positionZ, const RenderEngine::Rect crop, RenderEngine::Color4F color,
const RenderEngine::BlendFunc& blend, VideoRotation rotation, RenderEngine::Drawable2D::FlipMode flipMode)
{
    if (_textureDrawer) {
        _textureDrawer->draw(&_duplicateTexture, mvpMatrix, region, positionZ, crop, color, blend, flipMode);
    }
}

const RenderEngine::Texture2D* PixelFrameBGRADrawer::getOutputTexture() {
    return &_duplicateTexture;
}

void PixelFrameBGRADrawer::release()
{
    if (_textureDrawer) {
        _textureDrawer.reset();
    }
    clearTexture();
}


VideoFrameDrawer* VideoFrameDrawer::createVideoFrameDrawer(RawVideoFormat pixel_format) {
    return new PixelFrameNV12Drawer();
}
