//
//  IOSFastTexture.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <CoreVideo/CoreVideo.h>
#import <CoreGraphics/CoreGraphics.h>
#import "QGLContext.h"

@protocol IOSTexture <NSObject>
@property (nonatomic, readonly) GLuint glTexid;
@property (nonatomic, readonly) GLenum glTexTarget;
@property (nonatomic, readonly) size_t width;
@property (nonatomic, readonly) size_t height;
@end

#pragma mark -----IOSGeneralTexture------
@interface IOSGeneralTexture : NSObject <IOSTexture>

- (instancetype)initWithSize:(CGSize)size;

@property (nonatomic, readonly) GLuint glTexid;
@property (nonatomic, readonly) GLenum glTexTarget;
@property (nonatomic, readonly) size_t width;
@property (nonatomic, readonly) size_t height;

@end

#pragma mark -----IOSFastTexture------
@interface IOSFastTexture : NSObject <IOSTexture>

- (instancetype)initWithContext:(QGLContext*)context size:(CGSize)size pool:(CVPixelBufferPoolRef)bufferPool;
- (instancetype)initWithContext:(EAGLContext*)context size:(CGSize)size;
- (void)releaseTexture;

@property (nonatomic, readonly) CVPixelBufferRef pixelBuffer;
@property (nonatomic, readonly) GLuint glTexid;
@property (nonatomic, readonly) GLenum glTexTarget;

@property (nonatomic, readonly) size_t width;
@property (nonatomic, readonly) size_t height;

@end
