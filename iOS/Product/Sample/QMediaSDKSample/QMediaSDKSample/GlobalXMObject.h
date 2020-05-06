//
//  GlobalXMObject.h
//  QMediaSDKSample
//
//  Created by LINQIUMING on 23/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <QMediaSDK/QMediaSDK.h>

#define kGlobalXMObjectSubObjectArrayChanged    @"kGlobalXMObjectSubObjectArrayChanged"
#define kGlobalXMObjectPixelSizeChanged         @"kGlobalXMObjectPixelSizeChanged"

@interface GlobalXMObject : NSObject
@property (nonatomic) NSInteger selectedPixelSizeIndex;
@property (nonatomic, strong) QEditorPlayer* player;

+ (instancetype)sharedInstance;

@end
