//
//  FloatingTimeView.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface FloatingTimeView : UIView
@property (nonatomic) IBOutlet UILabel *timeLabel;
@property (nonatomic) CGFloat currentTime;

+ (instancetype)floatingTimeView;

@end
