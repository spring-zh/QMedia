/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2017 Chukong Technologies Inc.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "Utils/PlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS

#import "CCDirectorCaller-ios.h"

#import <Foundation/Foundation.h>
#import <OpenGLES/EAGL.h>

#import "CCEAGLView-ios.h"

static id s_sharedDirectorCaller;

@interface NSObject(CADisplayLink)
+(id) displayLinkWithTarget: (id)arg1 selector:(SEL)arg2;
-(void) addToRunLoop: (id)arg1 forMode: (id)arg2;
-(void) setFrameInterval: (NSInteger)interval;
-(void) invalidate;
@end

@implementation CCDirectorCaller{
    dispatch_queue_t renderQueue;
    dispatch_semaphore_t sem;
    NSRunLoop *runloop;
}

@synthesize interval;

+(id) sharedDirectorCaller
{
    if (s_sharedDirectorCaller == nil)
    {
        s_sharedDirectorCaller = [CCDirectorCaller new];
    }
    
    return s_sharedDirectorCaller;
}

+(void) destroy
{
    [s_sharedDirectorCaller stopMainLoop];
    s_sharedDirectorCaller = nil;
}

-(void) alloc
{
    interval = 1;
    eaglView = nil;
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        isAppActive = [UIApplication sharedApplication].applicationState == UIApplicationStateActive;
        NSNotificationCenter *nc = [NSNotificationCenter defaultCenter];
        [nc addObserver:self selector:@selector(appDidBecomeActive) name:UIApplicationDidBecomeActiveNotification object:nil];
        [nc addObserver:self selector:@selector(appDidBecomeInactive) name:UIApplicationWillResignActiveNotification object:nil];
    }
    return self;
}

-(void) dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    displayLink = nil;
}

- (void)appDidBecomeActive
{
    isAppActive = YES;
}

- (void)appDidBecomeInactive
{
    isAppActive = NO;
}

-(void) startMainLoop
{
        // Director::setAnimationInterval() is called, we should invalidate it first
    [self stopMainLoop];
    //TODO: change display framerate here. 1->60 , 2->30
    self.interval = 2;
    sem = dispatch_semaphore_create(0);
    renderQueue = dispatch_queue_create("com.glrender",DISPATCH_QUEUE_SERIAL);
    dispatch_async(renderQueue, ^(){
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(doCaller:)];
        [displayLink setFrameInterval: self.interval];
        
        runloop = [NSRunLoop currentRunLoop];
        [displayLink addToRunLoop:runloop forMode:NSRunLoopCommonModes];
        
        dispatch_semaphore_signal(sem);
        [runloop run];
        NSLog(@"renderQueue exit");
    });
    
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
}

-(void) stopMainLoop
{
    [displayLink invalidate];
    displayLink = nil;
    if(runloop != nil){
        CFRunLoopStop([runloop getCFRunLoop]);
        runloop = nil;
    }
    renderQueue = nil;
    sem = nil;
}

-(void) setAnimationInterval:(double)intervalNew
{
    // Director::setAnimationInterval() is called, we should invalidate it first
    [self stopMainLoop];
        
    self.interval = 30.0 * intervalNew;
        
    dispatch_async(renderQueue, ^(){
        displayLink = [NSClassFromString(@"CADisplayLink") displayLinkWithTarget:self selector:@selector(doCaller:)];
        [displayLink setFrameInterval: self.interval];
        
        runloop = [NSRunLoop currentRunLoop];
        [displayLink addToRunLoop:runloop forMode:NSRunLoopCommonModes];
        [runloop run];
        NSLog(@"renderQueue exit");
    });
}
                      
-(void) doCaller: (id) sender
{
    if (isAppActive && eaglView && [(CCEAGLView*)eaglView isDisplayedInScreen]) {
        [(CCEAGLView*)eaglView render];
    }
}

-(void)setEAGLView:(id)view
{
    eaglView = view;
}

@end

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_IOS

