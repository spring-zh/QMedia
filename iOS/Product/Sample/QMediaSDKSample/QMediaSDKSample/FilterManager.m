//
//  FilterManager.m
//  QMediaSDKSample
//
//  Created by LINQIUMING on 17/06/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "FilterManager.h"

@interface FilterManager ()
//@property (nonatomic, strong) NSMutableArray<XMFilter*>* allFiltersArray;
@end

@implementation FilterManager

+ (instancetype)sharedInstance
{
    static FilterManager* object = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        object = [FilterManager new];
    });
    return object;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
//        _allFiltersArray = [NSMutableArray new];
        
        // 加载预设的
//        for (NSInteger i = 1; i < XMPresetFilterId_MAX; ++i) {
//            XMFilter* filter = [XMFilter filterWithPresetFilterId:(XMPresetFilterId)i];
//            [_allFiltersArray addObject:filter];
//        }
//
//        // 加载文件的
//        NSString* filterPath = [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath], @"video/brightness"];
//        XMFilter* filter = [XMFilter filterWithFilePath:filterPath];
//        [_allFiltersArray addObject:filter];
    }
    return self;
}

//- (NSArray<XMFilter*>*)allFilters
//{
//    return _allFiltersArray;
//}

@end
