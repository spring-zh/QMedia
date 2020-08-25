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
#import "IOSTexture.h"
#import "IOSGLTextureDrawable.h"

@interface QPlayerView () <GLRenderer>
@end

@implementation QPlayerView {
    NSString *_outpath;
    bool _bTakePicture;
    CCEAGLView *_objView;
    QVideoDescribe* _describe;
    bool _bStart;
    bool _updateView;
    CGSize _viewSize;
    CGSize _targetSize;
    
    QDisplayMode _displayMode;
}

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
    _targetSize.width = 0;
    _targetSize.height = 0;
    _bStart = false;
    
    _displayMode = DisplayStretch;
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
    if (_bStart || _updateView) {
        _updateView = false;

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
}

- (void)setDisplayMode:(QDisplayMode)mode {
    _displayMode = mode;
    if (_videoRender) {
        [_videoRender setDisplayMode:mode viewW:_viewSize.width viewH:_viewSize.height];
    }
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
    _updateView = true;
}

- (void)layoutSubviews{
//    _viewSize = [self bounds].size;
    _viewSize.width = _objView.getWidth;
    _viewSize.height = _objView.getHeight;
    [_videoRender setDisplayMode:_displayMode viewW:_viewSize.width viewH:_viewSize.height];
    _updateView = true;
}

- (bool)initVideo:(QVideoDescribe*)describe
{
//    _iosTexture = [[IOSFastTexture alloc] initWithContext:_objView.context size:CGSizeMake(describe.width, describe.height)];
    _describe = describe;
    _targetSize = CGSizeMake(describe.width, describe.height);
    _updateView = true;
    return true;
}
- (int)getWidth
{
    return _describe.width;
}
- (int)getHeight
{
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
