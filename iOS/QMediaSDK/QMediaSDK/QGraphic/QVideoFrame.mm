
#include "QVideoFrame.h"
#include "Utils/Comm.h"
#include "GraphicCore/opengl/Drawable2D.h"
#include "GraphicCore/opengl/shaders/ccShaders.h"
#import <OpenGLES/EAGL.h>
#import <CoreFoundation/CoreFoundation.h>

USING_GRAPHICCORE

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
uniform highp vec4 uColor;

void main()
{
    mediump vec3 yuv;
    lowp vec3 rgb;
     
    // Subtract constants to map the video range start at 0
    yuv.x = (texture2D(SamplerY, v_texCoord).r - (16.0 / 255.0));
    yuv.yz = (texture2D(SamplerUV, v_texCoord).rg - vec2(0.5, 0.5));
    rgb = colorConversionMatrix * yuv;
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
    
    //TODO: need to add BGRA pixelbuffer supported
    
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
                                                       GL_RED_EXT,
                                                       frameWidth,
                                                       frameHeight,
                                                       GL_RED_EXT,
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
                                                           GL_RG_EXT,
                                                           frameWidth / 2,
                                                           frameHeight / 2,
                                                           GL_RG_EXT,
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
        if(_program->createProgram(defaultPositionTexture_vert, PositionTextureNV12_frag)){
            if(_program->use()){
                _program->addVertexAttribOption("a_position",VertexAttrib::VERTEX3);
                _program->addVertexAttribOption("a_texCoord",VertexAttrib::TEXCOORD);
                _program->addUniformOption("SamplerY",Uniform::TEXTURE);
                _program->addUniformOption("SamplerUV",Uniform::TEXTURE);
                _program->addUniformOption("uMVPMatrix",Uniform::MATRIX4);
                _program->addUniformOption("uTexMatrix",Uniform::MATRIX4);
                _program->addUniformOption("colorConversionMatrix",Uniform::MATRIX3);
                _program->addUniformOption("uColor", Uniform::FLOAT4);
               
            }
        }
    }
    
    return true;
}

void PixelFrameNV12Drawer::drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* node)
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
        _program->setUniformValue("SamplerY",(int)_textures[0]);
        _program->setUniformValue("SamplerUV",(int)_textures[1]);
        GraphicCore::Mat4 mvpMatrix;
        GraphicCore::Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
        GraphicCore::Mat4 texMatrix;
        _program->setUniformValue("uTexMatrix", 16, texMatrix.m/*Drawable2D::MtxFlipV*/);
        _program->setUniformValue("uMVPMatrix", 16, mvpMatrix.m);
        _program->setUniformValue("colorConversionMatrix", 16, _colorConversionMatrix);
        
        Uniform::Value colorVal;
        colorVal._floatOrmatrix_array = {
            node->getColor().r,
            node->getColor().g,
            node->getColor().b,
            node->getColor().a};
        _program->setUniformValue("uColor", colorVal);
        if (! FLOAT_ISEQUAL(node->getColor().a, 1.0f)) {
            _program->enableBlend(true);
        }
        else
            _program->enableBlend(false);
        
        _program->drawRect();
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
VideoFrameDrawer(videoTarget)
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
    
    //TODO: need to add BGRA pixelbuffer supported
    
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
//        glUniformMatrix3fv(_uniform[0], 1, GL_FALSE, kColorConversion601);
        _colorConversionMatrix = kColorConversion601;
    }else {
//        glUniformMatrix3fv(_uniform[0], 1, GL_FALSE, kColorConversion709);
        _colorConversionMatrix = kColorConversion709;
    }
    
    if (!_program) {
        _program = std::shared_ptr<ShaderProgram>(new ShaderProgram());
        if(_program->createProgram(defaultPositionTexture_vert, defaultPositionTexture_frag)){
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
    
//    if (!_shareTexture) {
//        _shareTexture = std::shared_ptr<GraphicCore::DuplicateTexture2D>(new GraphicCore::DuplicateTexture2D(_textures, Texture2D::RGBA, frameHeight, frameHeight));
//    }else{
//        _shareTexture->updateTexture(_textures, Texture2D::RGBA, frameHeight, frameHeight);
//    }
    
    return true;
}

void PixelFrameBGRADrawer::drawFrame(const GraphicCore::Scene* scene, const GraphicCore::Mat4 & transform, const GraphicCore::Node* node)
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
        _program->setUniformValue("uTexture",(int)_textures);
        GraphicCore::Mat4 mvpMatrix;
        GraphicCore::Mat4::multiply(scene->getMatrix(MATRIX_STACK_PROJECTION), transform, &mvpMatrix);
        GraphicCore::Mat4 texMatrix;
        _program->setUniformValue("uTexMatrix", 16, texMatrix.m/*Drawable2D::MtxFlipV*/);
        _program->setUniformValue("uMVPMatrix", 16, mvpMatrix.m);
        
        Uniform::Value colorVal;
        colorVal._floatOrmatrix_array = {
            node->getColor().r,
            node->getColor().g,
            node->getColor().b,
            node->getColor().a};
        _program->setUniformValue("uColor", colorVal);
        if (! FLOAT_ISEQUAL(node->getColor().a, 1.0f)) {
            _program->enableBlend(true);
        }
        else
            _program->enableBlend(false);
        
        _program->drawRect();
    }
}

//Texture2D* PixelFrameBGRADrawer::getDuplicateTexture()
//{
//    return _shareTexture.get();
//}

void PixelFrameBGRADrawer::release()
{
    if (_program) {
        _program->releaseProgram();
        _program.reset();
    }
    clearTexture();
}

