//
//  QDeviceUtils.m
//  QMediaSDK
//
//  Created by LINQIUMING on 05/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "QDeviceUtils.h"
#import <sys/utsname.h>

@implementation QDeviceUtils

+ (NSString *)deviceName {
    
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceName = [NSString stringWithCString:systemInfo.machine
                                              encoding:NSUTF8StringEncoding];
    return deviceName;
    
}

@end

@implementation QDeviceUtils (XMDeviceCamera)

+ (AVCaptureDevice *)cameraWithPosition:(AVCaptureDevicePosition)position {
    
    NSArray *deviceArray = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for(AVCaptureDevice *device in deviceArray){
        if(device.position == position){
            return device;
        }
    }
    return nil;
    
}

+ (NSInteger)cameraAmount {
    
    return [[AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo] count];
    
}

+ (BOOL)canSwitchCamera {
    
    return ([QDeviceUtils cameraAmount] > 1);
    
}

+ (BOOL)isCameraAvailable {
    
    AVAuthorizationStatus cameraStatus;
    cameraStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo];
    if (cameraStatus == AVAuthorizationStatusAuthorized){
        return YES;
    }
    return NO;
    
}

@end

@implementation QDeviceUtils (XMDeviceMicrophone)

+ (BOOL)isMicrophoneAvailable {
    
    AVAuthorizationStatus microphoneStatus;
    microphoneStatus = [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeAudio];
    if (microphoneStatus == AVAuthorizationStatusAuthorized){
        return YES;
    }
    return NO;
    
}

@end

@implementation QDeviceUtils (XMDeviceFlash)

+ (BOOL)isFlashAvailableForDevice:(AVCaptureDevice *)device {
    
    return [device hasFlash] && [device isFlashAvailable];
    
}

@end

@implementation QDeviceUtils (XMDeviceTorch)

+ (BOOL)isTorchAvailableForDevice:(AVCaptureDevice *)device {
    
    return [device hasTorch] && [device isTorchAvailable];
    
}

@end

@implementation QDeviceUtils (PixelDump)

+(UIImage*)convertBitmapRGBA8ToUIImage:(unsigned char *)buffer :(int)width :(int)height {

    CGDataProviderRef ref = CGDataProviderCreateWithData(NULL, buffer, width * height * 4, NULL);
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    CGImageRef iref = CGImageCreate(width, height, 8, 32, width * 4, colorspace, kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast, ref, NULL, true, kCGRenderingIntentDefault);

    float scale = 1.0f;


    UIGraphicsBeginImageContextWithOptions(CGSizeMake(width/scale, height/scale), NO, scale);
    CGContextRef cgcontext = UIGraphicsGetCurrentContext();

    CGContextSetBlendMode(cgcontext, kCGBlendModeCopy);

    // Image needs to be flipped BACK for CG
    CGContextTranslateCTM(cgcontext, 0, height/scale);
    CGContextScaleCTM(cgcontext, 1, -1);
    CGContextDrawImage(cgcontext, CGRectMake(0.0, 0.0, width/scale, height/scale), iref);

    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();

    UIGraphicsEndImageContext();

    CFRelease(ref);
    CFRelease(colorspace);
    CGImageRelease(iref);

    return image;
}

#pragma mark ------- this function can be called in c++ source file  ---------------
void DumpRGBAPixelToUIImage(unsigned char * buffer, int width, int height ) {
    UIImage *img = [QDeviceUtils convertBitmapRGBA8ToUIImage:buffer :width :height];
    //TODO: you can make breakpoint here to show image
    NSLog(@"breakpoint this log DumpRGBAPixelToUIImage %@", img);
}

@end
