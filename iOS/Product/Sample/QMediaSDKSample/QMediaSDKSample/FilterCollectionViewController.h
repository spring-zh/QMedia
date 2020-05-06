//
//  FilterCollectionViewController.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol FilterCollectionViewControllerDelegate <NSObject>
@optional
- (void)onFilterCollectionViewDidSelectedFilterAtIndex:(NSInteger)index;
@end

@interface FilterCollectionViewController : UICollectionViewController
@property (nonatomic,weak) id<FilterCollectionViewControllerDelegate> delegate;
@end
