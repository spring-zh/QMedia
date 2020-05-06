//
//  CommonKits.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 05/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "CommonKits.h"

@implementation CommonKits

+ (NSString *)formattedTimeStringWithFloat:(CGFloat)totalSeconds
{
    int minutes = ((int)totalSeconds / 60) % 60;
    CGFloat fSeconds = totalSeconds-minutes*60;
    
    return [NSString stringWithFormat:@"%02d:%.02f", minutes, fSeconds];
}

+ (NSString *)tmpPath
{
    static NSString* tmpPath;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        tmpPath = NSTemporaryDirectory();
    });
    return tmpPath;
}

+ (void)showAlertWithTitle:(NSString*)title message:(NSString*)message
{
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:title
                                                                   message:message
                                                            preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction* action = [UIAlertAction actionWithTitle:@"确认"
                                                     style:UIAlertActionStyleCancel
                                                   handler:^(UIAlertAction * _Nonnull action) {
                                                       
                                                   }];
    [alert addAction:action];
    
    [[UIApplication sharedApplication].delegate.window.rootViewController
     presentViewController:alert
     animated:YES
     completion:nil];
}

@end
