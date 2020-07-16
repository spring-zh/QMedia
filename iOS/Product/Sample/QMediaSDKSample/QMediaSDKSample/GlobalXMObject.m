//
//  GlobalXMObject.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 23/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "GlobalXMObject.h"
#import "PixelSizeManager.h"

@interface GlobalXMObject ()
@property (nonatomic, weak) PixelSizeManager* pixelSizeMngr;
@end

@implementation GlobalXMObject {
    NSMutableArray<id<QTrack>>* _tracks;
    NSMutableArray<id<GlobalMeidaManageObserver>>* _observers;
}

+ (instancetype)sharedInstance
{
    static GlobalXMObject* object = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        object = [GlobalXMObject new];
    });
    return object;
}

- (instancetype)init {
    if (self = [super init]) {
        _player = [[QEditorPlayer alloc] init];
        _tracks = [NSMutableArray new];
        _observers = [NSMutableArray new];
        self.pixelSizeMngr = [PixelSizeManager sharedInstance];
    }
    return self;
}

- (void)notifyTrackChange {
    for (id<GlobalMeidaManageObserver> observer in _observers) {
        if ([observer respondsToSelector:@selector(onTrackChange)]) {
            [observer onTrackChange];
        }
    }
}

- (void)notifySelectTrackChange:(NSInteger)index {
    for (id<GlobalMeidaManageObserver> observer in _observers) {
        if ([observer respondsToSelector:@selector(onSelectTrackAtIndex:)]) {
            [observer onSelectTrackAtIndex:index];
        }
    }
}

- (void)addTrack:(id<QTrack>)track {
    [_tracks addObject:track];
    [self notifyTrackChange];
}
- (void)removeTrack:(id<QTrack>)track {
    [_tracks removeObject:track];
    [self notifyTrackChange];
}
- (void)clearAllTrack {
    [_tracks removeAllObjects];
    [self notifyTrackChange];
}

- (NSMutableArray<id<QTrack>> *)tracks {
    return _tracks;
}

- (void)setSelectTrackIndex:(NSInteger)index {
    _selectedPixelSizeIndex = index;
    [self notifySelectTrackChange:index];
}

- (void)setSelectedPixelSizeIndex:(NSInteger)selectedPixelSizeIndex
{
    if (selectedPixelSizeIndex < 0 || selectedPixelSizeIndex > self.pixelSizeMngr.allPixelSizes.count-1) {
        return ;
    }
    if (CGSizeEqualToSize(self.pixelSize, self.pixelSizeMngr.allPixelSizes[selectedPixelSizeIndex].size)) {
        return ;
    }
    _selectedPixelSizeIndex = selectedPixelSizeIndex;

    self.pixelSize = self.pixelSizeMngr.allPixelSizes[_selectedPixelSizeIndex].size;

    [[NSNotificationCenter defaultCenter] postNotificationName:kGlobalXMObjectPixelSizeChanged object:self];
}

@end
