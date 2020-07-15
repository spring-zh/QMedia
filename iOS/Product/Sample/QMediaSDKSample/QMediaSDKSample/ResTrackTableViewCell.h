//
//  ResTrackTableViewCell.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ResTrackTableViewCell : UITableViewCell
@property (nonatomic) NSUInteger cellIndex;
@property (nonatomic) IBOutlet UIImageView* resThumbImageView;
@property (nonatomic) IBOutlet UILabel* resNameLabel;

@property (nonatomic) CGFloat sourceTimeLength; // 数据源的时长，单位是秒，允许有小数点
@property (nonatomic) CGFloat resTimeLength; // 资源自身的时长，单位是秒，允许有小数点
@property (nonatomic) CGFloat globalTimeLength; // 全局的时长设置，单位是秒，允许有小数点
@property (nonatomic) CGFloat resStartTimePoint; // 资源在全局的起始位置

@end
