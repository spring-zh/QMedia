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

@protocol GlobalMeidaManageObserver <NSObject>
-(void)onTrackChange;
-(void)onSelectTrackAtIndex:(NSInteger)index;
@end

@interface GlobalXMObject : NSObject
@property (nonatomic) NSInteger selectedPixelSizeIndex;
@property (nonatomic) CGSize pixelSize;
@property (nonatomic) NSInteger selectTrackIndex;
@property (nonatomic, strong) QEditorPlayer* player;

@property (nonatomic, readonly) NSArray<id<QTrack>>* tracks;

@property (nonatomic) NSMutableArray<id<GlobalMeidaManageObserver>>* observers;

- (void)addTrack:(id<QTrack>)track;
- (void)removeTrack:(id<QTrack>)track;
- (void)clearAllTrack;

+ (instancetype)sharedInstance;

@end
