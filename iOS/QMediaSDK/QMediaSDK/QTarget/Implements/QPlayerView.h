//
//  QPlayerView.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "QVideoRunloop.h"

typedef NS_ENUM(NSInteger, QDisplayMode) {
    ModeStretch = 0,
    ModeAdaptive,
    ModeClip
};

@interface QPlayerView : UIView <QVideoRunloop>

- (void)setDisplayMode:(QDisplayMode)mode;
- (bool)capturePicture:(NSString *)filePath;

@end
