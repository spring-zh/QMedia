//
//  QDeviceUtils.m
//  QMediaSDK
//
//  Created by LINQIUMING on 05/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

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
