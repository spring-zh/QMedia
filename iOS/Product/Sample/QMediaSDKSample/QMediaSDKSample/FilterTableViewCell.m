//
//  FilterTableViewCell.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "FilterTableViewCell.h"
#import "PreviewSliderBarController.h"
#import <QMediaSDK/QMediaSDK.h>

@interface FilterTableViewCell () <PreviewSliderBarControllerDelegate>
@property (nonatomic) IBOutlet UIView* sliderBarContainer;
@end

@implementation FilterTableViewCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
    
    UIStoryboard *mainStory = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    _sliderBarCtrl = [mainStory instantiateViewControllerWithIdentifier:@"PreviewSliderBarController"];
    _sliderBarCtrl.view.frame = _sliderBarContainer.bounds;
    _sliderBarCtrl.disableProgress = YES;
    _sliderBarCtrl.delegate = self;
    [self.sliderBarContainer addSubview:_sliderBarCtrl.view];
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void)onPreviewSliderBarControllerTrimLeftEnded:(PreviewSliderBarController*)sliderBarCtrl
{
//    XMTimeRange range = self.filter.timeRange;
//    range.startPoint = _sliderBarCtrl.trimLeftProgress*_sliderBarCtrl.totalTimeLength;
//    range.duration = (1-_sliderBarCtrl.trimLeftProgress-_sliderBarCtrl.trimRightProgress)*_sliderBarCtrl.totalTimeLength;
//    self.filter.timeRange = range;
}

- (void)onPreviewSliderBarControllerTrimRightEnded:(PreviewSliderBarController*)sliderBarCtrl
{
//    XMTimeRange range = self.filter.timeRange;
//    range.startPoint = _sliderBarCtrl.trimLeftProgress*_sliderBarCtrl.totalTimeLength;
//    range.duration = (1-_sliderBarCtrl.trimLeftProgress-_sliderBarCtrl.trimRightProgress)*_sliderBarCtrl.totalTimeLength;
//    self.filter.timeRange = range;
}

@end
