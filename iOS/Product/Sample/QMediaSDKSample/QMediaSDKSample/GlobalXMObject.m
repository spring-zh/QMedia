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

@implementation GlobalXMObject

+ (instancetype)sharedInstance
{
    static GlobalXMObject* object = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        object = [GlobalXMObject new];
//        object.pixelSizeMngr = [PixelSizeManager sharedInstance];
        object.player = [[QEditorPlayer alloc] init];
    });
    return object;
}

- (void)setSelectedPixelSizeIndex:(NSInteger)selectedPixelSizeIndex
{
//    if (selectedPixelSizeIndex < 0 || selectedPixelSizeIndex > self.pixelSizeMngr.allPixelSizes.count-1) {
//        return ;
//    }
//    if (CGSizeEqualToSize(self.pixelSize, self.pixelSizeMngr.allPixelSizes[selectedPixelSizeIndex].size)) {
//        return ;
//    }
//    _selectedPixelSizeIndex = selectedPixelSizeIndex;
//
//    self.pixelSize = self.pixelSizeMngr.allPixelSizes[_selectedPixelSizeIndex].size;
//    self.layout.frameSize = self.pixelSize;
//    self.layout.centerPoint = CGPointMake(self.pixelSize.width/2, self.pixelSize.height/2);
//
//    [[NSNotificationCenter defaultCenter] postNotificationName:kGlobalXMObjectPixelSizeChanged object:self];
}

//- (BOOL)addSubObject:(XMObject *)subObject
//{
////    BOOL ret = [super addSubObject:subObject];
////    if (ret) {
////        [[NSNotificationCenter defaultCenter] postNotificationName:kGlobalXMObjectSubObjectArrayChanged object:self];
////    }
////    return ret;
//    return FALSE;
//}

//- (BOOL)removeSubObject:(XMObject *)subObject
//{
////    BOOL ret = [super removeSubObject:subObject];
////    if (ret) {
////        [[NSNotificationCenter defaultCenter] postNotificationName:kGlobalXMObjectSubObjectArrayChanged object:self];
////    }
////    return ret;
//    return FALSE;
//}

- (BOOL)removeSubObjectAtIndex:(NSInteger)index
{
//    BOOL ret = [super removeSubObjectAtIndex:index];
//    if (ret) {
//        [[NSNotificationCenter defaultCenter] postNotificationName:kGlobalXMObjectSubObjectArrayChanged object:self];
//    }
//    return ret;
    return FALSE;
}

- (void)removeAllSubObjects
{
//    NSUInteger originalCount = [super subObjects].count;
//    [super removeAllSubObjects];
//    if (originalCount != [super subObjects].count) {
//        [[NSNotificationCenter defaultCenter] postNotificationName:kGlobalXMObjectSubObjectArrayChanged object:self];
//    }
}

- (BOOL)moveSubObjectFromIndex:(NSInteger)fromIndex toIndex:(NSInteger)toIndex
{
//    BOOL ret = [super moveSubObjectFromIndex:fromIndex toIndex:toIndex];
//    if (ret) {
//        [[NSNotificationCenter defaultCenter] postNotificationName:kGlobalXMObjectSubObjectArrayChanged object:self];
//    }
//    return ret;
    return FALSE;
}

@end
