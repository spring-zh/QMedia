//
//  ResTrackTableViewController.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 07/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ResTrackTableViewController : UITableViewController
@property (nonatomic, readonly) NSUInteger selectRow;

- (void)selectRowByIndex:(NSUInteger)selectRow;
@end
