//
//  PixelSizeManager.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 17/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "PixelSizeManager.h"

#pragma mark - PixelSizeItem
@interface PixelSizeItem ()
- (void)setSize:(CGSize)size;
@end
@implementation PixelSizeItem
+ (instancetype)itemWithSize:(CGSize)size
{
    PixelSizeItem* item = [PixelSizeItem new];
    item.size = size;
    return item;
}

- (void)setSize:(CGSize)size
{
    _size = size;
}

- (NSString*)name
{
    if (!_name) {
        _name = [NSString stringWithFormat:@"%.0f*%.0f", _size.width, _size.height];
    }
    return _name;
}
@end

#pragma mark - PixelSizeManager

@interface PixelSizeManager ()
@property (nonatomic, strong) NSMutableArray<PixelSizeItem*>* allPixelSizeArray;
@end

@implementation PixelSizeManager

+ (instancetype)sharedInstance
{
    static PixelSizeManager* object = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        object = [PixelSizeManager new];
    });
    return object;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        _allPixelSizeArray = [NSMutableArray new];
        
        CGSize screenSize = [UIScreen mainScreen].bounds.size;
        screenSize.width *= [UIScreen mainScreen].scale;
        screenSize.height *= [UIScreen mainScreen].scale;
        [_allPixelSizeArray addObject:[PixelSizeItem itemWithSize:screenSize]];
        [_allPixelSizeArray addObject:[PixelSizeItem itemWithSize:CGSizeMake(160, 160)]];
        [_allPixelSizeArray addObject:[PixelSizeItem itemWithSize:CGSizeMake(screenSize.height, screenSize.width)]];
        [_allPixelSizeArray addObject:[PixelSizeItem itemWithSize:CGSizeMake(MIN(screenSize.height, screenSize.width),
                                                                             MIN(screenSize.height, screenSize.width))]];
#define PSMADD_H(width,height) [_allPixelSizeArray addObject:[PixelSizeItem itemWithSize:CGSizeMake(width, height)]];
#define PSMADD_V(height,width) [_allPixelSizeArray addObject:[PixelSizeItem itemWithSize:CGSizeMake(width, height)]];
        PSMADD_H(1280, 720)
        PSMADD_H(1920, 1080)
        PSMADD_H(1280, 1024)
        PSMADD_H(800, 480)
        PSMADD_H(1024, 600)
        PSMADD_H(1280, 800)
        PSMADD_H(1366, 768)
        PSMADD_H(1440, 900)
        PSMADD_H(1680, 1050)
        PSMADD_H(1920, 1200)
        PSMADD_H(2560, 1600)
        PSMADD_H(640, 480)
        PSMADD_H(800, 600)
        PSMADD_H(1024, 768)
        PSMADD_H(1400, 1050)
        PSMADD_H(1600, 1200)
        PSMADD_H(2048, 1536)
        
        PSMADD_V(1280, 720)
        PSMADD_V(1920, 1080)
        PSMADD_V(1280, 1024)
        PSMADD_V(800, 480)
        PSMADD_V(1024, 600)
        PSMADD_V(1280, 800)
        PSMADD_V(1366, 768)
        PSMADD_V(1440, 900)
        PSMADD_V(1680, 1050)
        PSMADD_V(1920, 1200)
        PSMADD_V(2560, 1600)
        PSMADD_V(640, 480)
        PSMADD_V(800, 600)
        PSMADD_V(1024, 768)
        PSMADD_V(1400, 1050)
        PSMADD_V(1600, 1200)
        PSMADD_V(2048, 1536)
    }
    return self;
}

- (NSArray<PixelSizeItem*>*)allPixelSizes
{
    return _allPixelSizeArray;
}

@end
