//
//  PreviewSliderBarController.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>

@class PreviewSliderBarController;

@protocol PreviewSliderBarControllerDelegate <NSObject>
@optional
- (void)onPreviewSliderBarControllerDidLoad:(PreviewSliderBarController*)sliderBarCtrl;
- (void)onPreviewSliderBarControllerProgressBegin:(PreviewSliderBarController*)sliderBarCtrl;
- (void)onPreviewSliderBarControllerProgressChanged:(PreviewSliderBarController*)sliderBarCtrl;
- (void)onPreviewSliderBarControllerProgressRelease:(PreviewSliderBarController*)sliderBarCtrl;
- (void)onPreviewSliderBarControllerTrimLeftEnded:(PreviewSliderBarController*)sliderBarCtrl;
- (void)onPreviewSliderBarControllerTrimRightEnded:(PreviewSliderBarController*)sliderBarCtrl;
@end

@interface PreviewSliderBarController : UIViewController
@property (nonatomic, weak) id<PreviewSliderBarControllerDelegate> delegate;
@property (nonatomic) CGFloat progress;
@property (nonatomic) CGFloat trimLeftProgress;
@property (nonatomic) CGFloat trimRightProgress;
@property (nonatomic) CGFloat trimMinRemainPercent;
@property (nonatomic) CGFloat totalTimeLength; // 单位是秒，允许有小数点

@property (nonatomic) BOOL disableProgress;

@end
