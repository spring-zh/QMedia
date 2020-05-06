//
//  FloatingTimeView.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "FloatingTimeView.h"
#import "CommonKits.h"

@implementation FloatingTimeView

+ (instancetype)floatingTimeView
{
    NSArray * objects = [[NSBundle mainBundle] loadNibNamed:@"FloatingTimeView"
                                                      owner:nil
                                                    options:nil];
    if (objects.count > 0) {
        return objects[0];
    }
    return nil;
}

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    self.layer.cornerRadius = 3.0;
    self.layer.borderWidth = 1;
    self.layer.borderColor = [UIColor lightGrayColor].CGColor;
}

- (void)setCurrentTime:(CGFloat)currentTime
{
    _currentTime = currentTime;
    
    self.timeLabel.text = [CommonKits formattedTimeStringWithFloat:_currentTime];
}

@end
