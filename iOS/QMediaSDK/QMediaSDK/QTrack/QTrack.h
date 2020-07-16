//
//  QTrack.h
//  QMediaSDK
//
//  Created by spring on 2020/7/16.
//  Copyright © 2020 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol QTrack <NSObject>

@property (nonatomic, readonly) NSString* uid;
@property (nonatomic, readonly) NSString* displayName;
@property (nonatomic) QTimeRange sourceRange;
@property (nonatomic) QTimeRange displayRange;

@end
