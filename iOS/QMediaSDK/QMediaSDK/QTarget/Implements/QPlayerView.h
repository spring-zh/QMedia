//
//  QPlayerView.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "QVideoTarget.h"

@interface QPlayerView : UIView <QVideoTarget>

-(bool)capturePicture:(NSString *)filePath;

@end
