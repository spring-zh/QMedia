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

@property (nonatomic, readonly) NSArray<QMediaSegment*>* tracks;

@property (nonatomic) NSMutableArray<id<GlobalMeidaManageObserver>>* observers;

- (void)addTrack:(QMediaSegment*)track;
- (void)removeTrack:(QMediaSegment*)track;
- (void)clearAllTrack;

- (void)updateDisplay;

+ (instancetype)sharedInstance;

@end
