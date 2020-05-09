//
//  QPlayerView.m
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QPlayerView.h"

#import "OpenGL/CCEAGLView-ios.h"
#import "OpenGL/CCDirectorCaller-ios.h"
#import "QGLContext.h"
#import "IOSFastTexture.h"
#import "IOSFastTextureDrawable.h"

@interface QPlayerView () <GLRenderer>{

    NSString *_outpath;
    bool _bTakePicture;
    CCEAGLView *_objView;
    QVideoDescribe* _describe;
    bool _bStart;
    CGSize _viewSize;
    IOSFastTexture *_iosTexture;
    IOSFastTextureDrawable *_textureDrawable;
}

@end

@implementation QPlayerView

- (void)internalInit
{
    NSLog(@"QPlayerView internalInit %@", [NSThread currentThread]);
    
    CCEAGLView* subView = [CCEAGLView viewWithFrame: self.bounds
                                        pixelFormat: kEAGLColorFormatRGBA8
                                        depthFormat: GL_DEPTH_COMPONENT16
                                 preserveBackbuffer: NO
                                         sharegroup: [[[QGLContext sharedGLContext] context] sharegroup]
                                      multiSampling: NO
                                    numberOfSamples: 0 ];
    [subView setRenderer:self];
    subView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    subView.backgroundColor = [UIColor blackColor];
    [self addSubview:subView];
    _bTakePicture = false;
    _objView =subView ;
    _viewSize.width = subView.getWidth;
    _viewSize.height = subView.getHeight;
    _bStart = false;
}

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        [self internalInit];
    }
    return self;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        [self internalInit];
    }
    return self;
}

- (void)dealloc
{
    [_objView setRenderer:nil];
    NSLog(@"QPlayerView dealloc %@", [NSThread currentThread]);
    
}

- (void)onCreate
{
    [_videoRender onVideoCreate];
}

- (void)onDrawFrame
{
//    NSLog(@"QPlayerView onDrawFrame %@", [NSThread currentThread]);
    if (_bStart) {
        if (_iosTexture) {
            [IOSFastTextureDrawable savePrevStatus];
            if(!_textureDrawable)
                _textureDrawable = [[IOSFastTextureDrawable alloc] initWithTexture:_iosTexture];
            [_textureDrawable useAsFrameBuffer];
            [_videoRender onVideoRender:-1];
            [IOSFastTextureDrawable loadPrevStatus];
            glFinish();
            [_textureDrawable draw];
        }
        else
            [_videoRender onVideoRender:-1];
    }
    
    if (_bTakePicture) {
        [self capturePicture_direct:_outpath];
        _bTakePicture = false;
    }
}

- (void)onRelease
{
    [_videoRender onVideoDestroy];
    //TODO: release gl resource
    [_textureDrawable releaseFb];
    _textureDrawable = nil;
    _iosTexture = nil;
}

- (bool)capturePicture:(NSString *)filePath
{
    _bTakePicture = true;
    _outpath = [filePath copy];
    return true;
}

-(bool)capturePicture_direct:(NSString *)filePath
{
    CGSize size = self.bounds.size;
    
    UIGraphicsBeginImageContextWithOptions(size, NO, [UIScreen mainScreen].scale);
    CGRect rec = CGRectMake(self.bounds.origin.x, self.bounds.origin.y, self.bounds.size.width, self.bounds.size.height);
    [self drawViewHierarchyInRect:rec afterScreenUpdates:YES];
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    NSData * data = UIImageJPEGRepresentation(image,1.0f);
    
//    NSArray *path = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
//    NSString *filename = [[path objectAtIndex:0] stringByAppendingPathComponent:@"foo.png"];
    [data writeToFile:filePath atomically:YES];
    return true;
}

#pragma mask XMVideoTarget implementation

@synthesize videoRender = _videoRender;

- (bool)startVideo{
    _bStart = true;
    return true;
}
- (bool)stopVideo
{
    _bStart = false;
    return true;
}
- (void)pauseVideo:(bool)isPause{
    _bStart = !isPause;
}
- (void)flushVideo{
    
}

- (void)layoutSubviews{
//    _viewSize = [self bounds].size;
}

- (bool)initVideo:(QVideoDescribe*)describe
{
    _iosTexture = [[IOSFastTexture alloc] initWithContext:_objView.context size:CGSizeMake(describe.width, describe.height)];
    _describe = describe;
    return true;
}
- (int)getWidth
{
    if (_iosTexture) {
        return _iosTexture.width;
    }
    return _describe.width;
}
- (int)getHeight
{
    if (_iosTexture) {
        return _iosTexture.height;
    }
    return _describe.height;
}
- (float)getFrameRate
{
    return _describe.framerate;
}
- (id)getContext{
    return _objView.context;
}

@end
