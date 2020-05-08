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

AnimaNode::AnimatorRef QNodeAnimatorToCPP(QNodeAnimator* animator){
    
    Range<int> range = {static_cast<int>(animator.timeRang.location), static_cast<int>(animator.timeRang.length + animator.timeRang.length)};
    AnimaNode::AnimatorData bData(GraphicCore::Vec4(animator.beginPoint.v0,animator.beginPoint.v1,animator.beginPoint.v2,animator.beginPoint.v3));
    AnimaNode::AnimatorData eData(GraphicCore::Vec4(animator.endPoint.v0,animator.endPoint.v1,animator.endPoint.v2,animator.endPoint.v3));
    return AnimaNode::AnimatorRef(new AnimaNode::Animator([animator.property UTF8String], range, bData, eData, (GraphicCore::EaseFunctionType)animator.functionType, animator.repleat ,[animator.name UTF8String]));
}

@interface QNodeAnimator(internal)
@property (nonatomic, readonly) AnimaNode::Animator* native;
@end

@implementation QNodeAnimator {
    AnimaNode::AnimatorRef _animator;
}

- (instancetype)initWith:(NSString*)property range:(NSRange)timeRang begin:(QVector)beginPoint end:(QVector)endPoint functype:(QEaseFunction)functionType repleat:(bool)repleat
{
    return [self initWith:property range:timeRang begin:beginPoint end:endPoint functype:functionType repleat:repleat name:@""];
}
- (instancetype)initWith:(NSString*)property range:(NSRange)timeRang begin:(QVector)beginPoint end:(QVector)endPoint functype:(QEaseFunction)functionType repleat:(bool)repleat name:(NSString*)name
{
    if ((self = [super init]) != nil) {
        _property = property;
        _timeRang = timeRang;
        _beginPoint = beginPoint;
        _endPoint = endPoint;
        _functionType = functionType;
        _repleat = repleat;
        _name = name;
        _animator = QNodeAnimatorToCPP(self);
    }
    return self;
}

- (AnimaNode::Animator *)native {
    return _animator.get();
}

@end


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
    NSMutableArray* _animators;
}

- (instancetype)initWithName:(NSString*)name
{
    if ((self = [super init]) != nil) {
        _graphicNode = GraphicCore::RenderNodeRef(new GraphicCore::RenderNode());
        _nodeName = [name copy];
        std::string s_name = std::string([name UTF8String]);
        _graphicNode->setName(s_name);
        _childrens = [[NSMutableArray alloc] init];
        _animators = [[NSMutableArray alloc] init];
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
        _animators = [[NSMutableArray alloc] init];
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

- (NSArray*)animators{
    return _animators;
}

- (bool)addAnimator:(QNodeAnimator*)animator {
    if([_animators containsObject:animator])
        return true;
    [_animators addObject:animator];
    _graphicNode->addAnimator(animator.native);

    return false;
}
- (bool)removeAnimator:(QNodeAnimator*)animator {
    if([_animators containsObject:animator])
    {
        _graphicNode->removeAnimator(animator.native);
        [_animators removeObject:animator];
        return true;
    }
    return false;
}
- (void)clearAllAnimators {
    for (QNodeAnimator* animator in _animators) {
        _graphicNode->removeAnimator(animator.native);
    }
    [_animators removeAllObjects];
}

- (NSString*)name{
//    return [NSString stringWithUTF8String:_graphicNode->getName().c_str()];
    return _nodeName;
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
