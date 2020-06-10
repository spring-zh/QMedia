//
//  QDuplicateNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QDuplicateNode.h"
#import "QGraphicNode_internal.h"

@implementation QDuplicateNode {
    GraphicCore::DuplicateNodeRef _duplicateNode;
    __weak QGraphicNode* _nodeFrom;
}

- (instancetype)initFromNode:(QGraphicNode*)nodeFrom combiner:(QCombiner*)combiner
{
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initFromNode:nodeFrom combiner:combiner uid:(__bridge_transfer NSString *)uuid];
}

- (instancetype)initFromNode:(QGraphicNode*)nodeFrom combiner:(QCombiner*)combiner uid:(NSString*)uid
{
    _nodeFrom = nodeFrom;
    _duplicateNode = GraphicCore::DuplicateNodeRef(new GraphicCore::DuplicateNode(nodeFrom.native.get()));
    return (self = [super initWithNode:_duplicateNode combiner:combiner uid:uid]);
}

- (QGraphicNode*)nodeFrom {
    return _nodeFrom;
}

- (QColor4)bkColor{
    GraphicCore::Color4F color = _duplicateNode->getBKColor();
    return QColorMaker(color.r, color.g, color.g, color.b);
}
- (void)setBkColor:(QColor4)bkColor{
    GraphicCore::Color4F color(bkColor.r,bkColor.g,bkColor.b,bkColor.a);
    _duplicateNode->setBKColor(color);
}

@end
