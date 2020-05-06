//
//  IOSFastTexture.m
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "IOSFastTexture.h"

@implementation IOSFastTexture {
    EAGLContext* _context;
    CVOpenGLESTextureCacheRef _textureCacheRef;
    CVPixelBufferRef _pixelBufferRef;
    CVOpenGLESTextureRef _textureRef;
    GLuint _glTexid;
    bool _isSharedTextureCache;
}

- (instancetype)initWithContext:(QGLContext*)context size:(CGSize)size pool:(CVPixelBufferPoolRef)bufferPool
{
    if (self = [super init]) {
        _context = context.context;
        _textureCacheRef = [context coreVideoTextureCache];
        _isSharedTextureCache = true;
        if (_textureCacheRef != NULL) {
            _glTexid = [self createGLTexture:size.width height:size.height pool:bufferPool];
        }
    }
    return self;
}

- (instancetype)initWithContext:(EAGLContext*)context size:(CGSize)size
{
    if (self = [super init]) {
        _context = context;
        _textureCacheRef = [IOSFastTexture createVideoTextureCache:_context];
        _isSharedTextureCache = false;
        if (_textureCacheRef != NULL) {
            _glTexid = [self createGLTexture:size.width height:size.height pool:nil];
        }
    }
    return self;
}

- (void)dealloc
{
    [self releaseTexture];
}

-(CVPixelBufferRef)pixelBuffer {
    return _pixelBufferRef;
}

- (GLuint)glTexid
{
    return _glTexid;
}

- (GLenum)glTexTarget
{
    return CVOpenGLESTextureGetTarget(_textureRef);
}

- (size_t)width
{
    return CVPixelBufferGetWidth(_pixelBufferRef);
}

- (size_t)height
{
    return CVPixelBufferGetHeight(_pixelBufferRef);
}

//FIXME: we can reuse CVOpenGLESTextureCacheRef
+ (CVOpenGLESTextureCacheRef)createVideoTextureCache:(EAGLContext*)context;
{
    CVOpenGLESTextureCacheRef coreVideoTextureCache = NULL;
#if defined(__IPHONE_6_0)
    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, context, NULL, &coreVideoTextureCache);
#else
    CVReturn err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, (__bridge void *)context, NULL, &_coreVideoTextureCache);
#endif

    if (err)
    {
        NSAssert(NO, @"Error at CVOpenGLESTextureCacheCreate %d", err);
    }

    return coreVideoTextureCache;
}

- (GLint)createGLTexture:(int)width height:(int)height pool:(CVPixelBufferPoolRef)bufferPool
{
    GLint texId = 0;
    CVReturn ret;
    if (bufferPool != NULL) {
        if(kCVReturnSuccess == (ret = CVPixelBufferPoolCreatePixelBuffer(kCFAllocatorDefault, bufferPool, &_pixelBufferRef))){
            /* AVAssetWriter will use BT.601 conversion matrix for RGB to YCbCr conversion
             * regardless of the kCVImageBufferYCbCrMatrixKey value.
             * Tagging the resulting video file as BT.601, is the best option right now.
             * Creating a proper BT.709 video is not possible at the moment.
             */
            CVBufferSetAttachment(_pixelBufferRef, kCVImageBufferColorPrimariesKey, kCVImageBufferColorPrimaries_ITU_R_709_2, kCVAttachmentMode_ShouldPropagate);
            CVBufferSetAttachment(_pixelBufferRef, kCVImageBufferYCbCrMatrixKey, kCVImageBufferYCbCrMatrix_ITU_R_601_4, kCVAttachmentMode_ShouldPropagate);
            CVBufferSetAttachment(_pixelBufferRef, kCVImageBufferTransferFunctionKey, kCVImageBufferTransferFunction_ITU_R_709_2, kCVAttachmentMode_ShouldPropagate);

        }
    }else {
        NSDictionary* pixelBufferOptions = @{ (NSString *) kCVPixelBufferOpenGLESCompatibilityKey : @YES,
        (NSString *) kCVPixelBufferIOSurfacePropertiesKey : @{}};
        ret = CVPixelBufferCreate(kCFAllocatorDefault, width, height, kCVPixelFormatType_32BGRA, (__bridge CFDictionaryRef)pixelBufferOptions, &_pixelBufferRef);
    }

    if(kCVReturnSuccess == ret)
    {
        CVReturn ret = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault, _textureCacheRef, _pixelBufferRef, nil, GL_TEXTURE_2D, GL_RGBA, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0, &_textureRef);
        if(kCVReturnSuccess != ret)
        {
            NSAssert(NO, @"CVOpenGLESTextureCacheCreateTextureFromImage() failed!, return code = %d", ret);
        }else
            texId = CVOpenGLESTextureGetName(_textureRef);
    }else
        NSAssert(NO,@"CVPixelBufferCreate() failed!, return code = %d", ret);
    return texId;
}

- (void)releaseTexture
{
    if (_textureRef != NULL) {
        CFRelease(_textureRef);
        _textureRef = NULL;
    }
    if (_pixelBufferRef != NULL) {
        CVPixelBufferRelease(_pixelBufferRef);
        _pixelBufferRef = NULL;
    }
    if (_textureCacheRef && !_isSharedTextureCache) {
        CVOpenGLESTextureCacheFlush(_textureCacheRef, 0);
        _textureCacheRef = NULL;
    }
    _glTexid = 0;
}

@end
