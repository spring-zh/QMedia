//
//  QImageNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QImageNode.h"
#import "QGraphicNode_internal.h"
#include "GraphicCore/GcImageNode.h"

@implementation QImageNode {
    NSString* _filePath;
    GraphicCore::ImageNodeRef _imageNode;
}

- (instancetype)initWithPath:(NSString*)filePath combiner:(QCombiner*)combiner
{
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initWithPath:filePath combiner:combiner uid:(__bridge_transfer NSString *)uuid];
}

- (instancetype)initWithPath:(NSString*)filePath combiner:(QCombiner*)combiner uid:(NSString*)uid
{
    _filePath = [filePath copy];
    _imageNode = GraphicCore::ImageNodeRef(new GraphicCore::ImageNode([filePath UTF8String]));
    return (self = [super initWithNode:_imageNode combiner:combiner uid:uid]);
}

- (void)dealloc {
    NSLog(@"QImageNode dealloc");
}

- (NSString*)filePath {
    return _filePath;
}

- (CGSize)imgSize {
    return CGSizeMake(_imageNode->getWidth(), _imageNode->getHeight());
}

@end
