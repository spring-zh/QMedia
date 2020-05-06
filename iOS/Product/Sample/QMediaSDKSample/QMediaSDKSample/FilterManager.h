//
//  FilterManager.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 17/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <QMediaSDK/QMediaSDK.h>

@interface FilterManager : NSObject
//@property (nonatomic, readonly) NSArray<XMFilter*>* allFilters;

// 由于目前Filter的range问题，不能搞共享，不然会有问题
// + (instancetype)sharedInstance;

@end
