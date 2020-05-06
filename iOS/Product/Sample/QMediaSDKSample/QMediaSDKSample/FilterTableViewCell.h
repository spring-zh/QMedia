//
//  FilterTableViewCell.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>

@class PreviewSliderBarController;
@class XMFilter;

@interface FilterTableViewCell : UITableViewCell
@property (nonatomic) IBOutlet UILabel* filterNameLabel;
@property (nonatomic, readonly) PreviewSliderBarController* sliderBarCtrl;
@property (nonatomic, weak) XMFilter* filter;
@end
