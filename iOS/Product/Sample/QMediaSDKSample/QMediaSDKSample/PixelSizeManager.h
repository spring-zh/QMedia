//
//  PixelSizeManager.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 17/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface PixelSizeItem : NSObject
@property (nonatomic, copy) NSString* name;
@property (nonatomic, readonly) CGSize size;
+ (instancetype)itemWithSize:(CGSize)size;
@end

@interface PixelSizeManager : NSObject
@property (nonatomic, readonly) NSArray<PixelSizeItem*>* allPixelSizes;

+ (instancetype)sharedInstance;

@end
