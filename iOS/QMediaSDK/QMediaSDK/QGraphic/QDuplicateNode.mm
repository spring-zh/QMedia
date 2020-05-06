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
}

- (instancetype)initFromNode:(QGraphicNode*)nodeFrom
{
    _duplicateNode = GraphicCore::DuplicateNodeRef(new GraphicCore::DuplicateNode(nodeFrom.native.get()));
    return (self = [super initWithNode:_duplicateNode]);
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
