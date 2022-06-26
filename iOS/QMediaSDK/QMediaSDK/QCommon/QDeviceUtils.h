//
//  QDeviceUtils.h
//  QMediaSDK
//
//  Created by LINQIUMING on 05/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface QDeviceUtils : NSObject

/**
 获取设备型号
 
 @return 设备型号
 */
+ (NSString *)deviceName;

@end

@interface QDeviceUtils (XMDeviceCamera)

/**
 获取指定位置的摄像头
 
 @param position 摄像头位置
 
 @return 成功返回AVCaptureDevice实例，失败返回nil
 */
+ (AVCaptureDevice *)cameraWithPosition:(AVCaptureDevicePosition)position;
/**
 获取摄像头数量
 
 @return 摄像头数量，无摄像头返回0
 */
+ (NSInteger)cameraAmount;
/**
 判断设备是否允许切换摄像头
 
 @return 允许返回YES，禁止返回NO
 */
+ (BOOL)canSwitchCamera;
/**
 判断是否允许访问摄像头
 
 @return 允许返回YES，禁止返回NO
 */
+ (BOOL)isCameraAvailable;

@end

@interface QDeviceUtils (XMDeviceMicrophone)

/**
 判断是否允许访问麦克风
 
 @return 允许返回YES，禁止返回NO
 */
+ (BOOL)isMicrophoneAvailable;

@end

@interface QDeviceUtils (XMDeviceFlash)

/**
 判断设备是否可用闪光灯
 
 @param device 设备
 
 @return 可用返回YES，不可用返回NO
 */
+ (BOOL)isFlashAvailableForDevice:(AVCaptureDevice *)device;

@end

@interface QDeviceUtils (XMDeviceTorch)

/**
 判断设备是否可用手电筒
 
 @param device 设备
 
 @return 可用返回YES，不可用返回NO
 */
+ (BOOL)isTorchAvailableForDevice:(AVCaptureDevice *)device;

@end

@interface QDeviceUtils (PixelDump)
+ (UIImage*)convertBitmapRGBA8ToUIImage:(unsigned char *)buffer :(int)width :(int)height;
@end
