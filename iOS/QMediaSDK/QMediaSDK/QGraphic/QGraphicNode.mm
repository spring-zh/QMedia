//
//  QGraphicNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QGraphicNode_internal.h"

using GraphicCore::AnimaNode;

QColor4 QColorMaker(float r, float g, float b, float a) {
    QColor4 color4;
    color4.r = r; color4.g = g; color4.b = b; color4.a = a;
    return color4;
}


NSString* property_positionxyz = @"positionxyz";
NSString* property_scalex = @"scalex";
NSString* property_scalez = @"scalez";
NSString* property_scalexy = @"scalexy";
NSString* property_scalexyz = @"scalexyz";
NSString* property_contentsizew = @"contentsizew";
NSString* property_contentsizeh = @"contentsizeh";
NSString* property_contentsize = @"contentsize";
NSString* property_alpha = @"alpha";
NSString* property_mixcolor = @"mixcolor";

@implementation QGraphicNode {
    NSString *_nodeName;
    GraphicCore::RenderNodeRef _graphicNode;
    NSMutableArray* _childrens;
}

- (instancetype)initWithName:(NSString*)name
{
    if ((self = [super init]) != nil) {
        _graphicNode = GraphicCore::RenderNodeRef(new GraphicCore::RenderNode());
        _nodeName = [name copy];
        std::string s_name = std::string([name UTF8String]);
        _graphicNode->setName(s_name);
        _childrens = [[NSMutableArray alloc] init];
    }
    return self;
}

#pragma mark - private function
- (instancetype)initWithNode:(GraphicCore::RenderNodeRef)graphicNode
{
    if ((self = [super init]) != nil) {
        _graphicNode = graphicNode;
        if (_graphicNode == nullptr) {
            return nil;
        }
        _nodeName = [NSString stringWithUTF8String:graphicNode->getName().c_str()];
        _childrens = [[NSMutableArray alloc] init];
    }
    return self;
}

- (GraphicCore::RenderNodeRef)native {
    return _graphicNode;
}
#pragma mark - private function //

- (NSArray*)childrens{
    return _childrens;
}

- (bool)addChildNode:(QGraphicNode*)childNode
{
    if (childNode == nil) {
        return false;
    }
    if([_childrens containsObject:childNode])
        return true;
    childNode.native->removeFromParent();
    childNode.native->setParent(nullptr);
    if(_graphicNode->addChildren(childNode.native.get())){
        [_childrens addObject:childNode];
        return true;
    }
    return false;
}
- (bool)removeChildNode:(QGraphicNode*)childNode
{
    if([_childrens containsObject:childNode])
    {
        childNode.native->removeFromParent();
        childNode.native->setParent(nullptr);
        [_childrens removeObject:childNode];
        return true;
    }
    return false;
}
- (void)clearAllChildrens
{
    for (QGraphicNode* child in _childrens) {
        child.native->removeFromParent();
        child.native->setParent(nullptr);
    }
    [_childrens removeAllObjects];
}

- (NSString*)name{
//    return [NSString stringWithUTF8String:_graphicNode->getName().c_str()];
    return _nodeName;
}

- (void)addAnimator:(NSString*)property range:(NSRange)timeRang begin:(QVector)beginData end:(QVector)endData functype:(QEaseFunction)functionType name:(NSString*)name
{
    Range<int> range = {static_cast<int>(timeRang.location), static_cast<int>(timeRang.length + timeRang.length)};
    AnimaNode::AnimatorData bData(GraphicCore::Vec4(beginData.v0,beginData.v1,beginData.v2,beginData.v3));
    AnimaNode::AnimatorData eData(GraphicCore::Vec4(endData.v0,endData.v1,endData.v2,endData.v3));
    _graphicNode->addAnimator([property UTF8String], range, bData, eData, (GraphicCore::EaseFunctionType)functionType ,[name UTF8String]);
}

- (NSRange)renderRange {
    auto range = _graphicNode->getRange();
    return NSMakeRange(range._start, range._end - range._start);
}

- (void)setRenderRange:(NSRange)renderRange {
    _graphicNode->setRange(Range<int64_t>(renderRange.location, renderRange.location + renderRange.length));
}

- (bool)visible{
    return _graphicNode->isVisible();
}
- (void)setVisible:(bool)visible{
    _graphicNode->setVisible(visible);
}
- (float)rotation{
    return _graphicNode->getRotation();
}
- (void)setRotation:(float)rotation{
    _graphicNode->setRotation(rotation);
}
- (float)scaleX{
    return _graphicNode->getScaleX();
}
- (void)setScaleX:(float)scaleX{
    _graphicNode->setScaleX(scaleX);
}
- (float)scaleY{
    return _graphicNode->getScaleY();
}
- (void)setScaleY:(float)scaleY{
    _graphicNode->setScaleY(scaleY);
}
- (float)scaleZ{
    return _graphicNode->getScaleZ();
}
- (void)setScaleZ:(float)scaleZ{
    _graphicNode->setScaleZ(scaleZ);
}

- (CGSize)contentSize{
    GraphicCore::Size sz = _graphicNode->getContentSize();
    return CGSizeMake(sz.width,sz.height);
}
- (void)setContentSize:(CGSize)contentSize{
    GraphicCore::Size sz(contentSize.width,contentSize.height);
    _graphicNode->setContentSize(sz);
}

- (CGPoint)position{
    GraphicCore::Vec2 pos = _graphicNode->getPosition();
    return CGPointMake(pos.x, pos.y);
}
- (void)setPosition:(CGPoint)position{
    GraphicCore::Vec2 pos(position.x,position.y);
    _graphicNode->setPosition(pos);
}
- (float)positionZ{
    return _graphicNode->getPositionZ();
}
- (void)setPositionZ:(float)positionZ{
    _graphicNode->setPositionZ(positionZ);
}

- (CGPoint)anchorPoint{
    GraphicCore::Vec2 anchor = _graphicNode->getAnchorPoint();
    return CGPointMake(anchor.x, anchor.y);
}
- (void)setAnchorPoint:(CGPoint)anchorPoint{
    GraphicCore::Vec2 anchor(anchorPoint.x,anchorPoint.y);
    _graphicNode->setAnchorPoint(anchor);
}

- (QColor4)color4{
    GraphicCore::Color4F color = _graphicNode->getColor();
    return QColorMaker(color.r, color.g, color.g, color.b);
}
- (void)setColor4:(QColor4)color4{
    GraphicCore::Color4F color(color4.r,color4.g,color4.b,color4.a);
    _graphicNode->setColor(color);
}

- (float)alpha{
    GraphicCore::Color4F color = _graphicNode->getColor();
    return color.a;
}
- (void)setAlpha:(float)alpha{
    GraphicCore::Color4F color(1,1,1,alpha);
    _graphicNode->setColor(color);
}

@end
