#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

//#include "Platform/CCGL.h"
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

extern EAGLRenderingAPI DefaultRenderingAPI;

@interface QGLContext : NSObject

@property(readonly, retain, nonatomic) EAGLContext *context;
@property(readonly) CVOpenGLESTextureCacheRef coreVideoTextureCache;

+ (QGLContext *)sharedGLContext;
+ (void)useSharedGLContext;
- (void)useAsCurrentContext;
+ (BOOL)deviceSupportsOpenGLESExtension:(NSString *)extension;
+ (BOOL)deviceSupportsRedTextures;
+ (BOOL)deviceSupportsFramebufferReads;

- (void)useSharegroup:(EAGLSharegroup *)sharegroup;

// Manage fast texture upload
+ (BOOL)supportsFastTextureUpload;

@end
