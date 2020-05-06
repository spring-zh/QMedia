//
//  CommonKits.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 05/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface CommonKits : NSObject

+ (NSString *)formattedTimeStringWithFloat:(CGFloat)totalSeconds;
+ (NSString *)tmpPath;

+ (void)showAlertWithTitle:(NSString*)title message:(NSString*)message;

@end
