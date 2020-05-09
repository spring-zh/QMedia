//
//  QGraphicNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QGraphicNode_internal.h"

using GraphicCore::AnimaNode;

@interface QNodeAnimator(internal)
@property (nonatomic, readonly) AnimaNode::Animator* native;
@end

QColor4 QColorMaker(float r, float g, float b, float a) {
    QColor4 color4;
    color4.r = r; color4.g = g; color4.b = b; color4.a = a;
    return color4;
}

#pragma mark QGraphicNode
@implementation QGraphicNode_Serialize
- (instancetype)init{
    if ((self = [super init]) != nil) {
        _origin_renderRange = NSMakeRange(0, 0);
        _origin_visible = false;
        _origin_rotation = 0;
        _origin_scaleX = 1.0f;
        _origin_scaleY = 1.0f;
        _origin_scaleZ = 1.0f;
        _origin_contentSize = CGSizeMake(0, 0);
        _origin_position = CGPointMake(0, 0);
        _origin_positionZ = 0;
        _origin_anchorPoint = CGPointMake(0, 0);
        _origin_color4 = QColorMaker(1, 1, 1, 1);
        _origin_alpha = 1.0f;
        _origin_visible = true;
    }
    return self;
}
- (NSDictionary*)serialize {
    NSDictionary* dic = [NSMutableDictionary new];
//    [dic setValue:@"QGraphicNode" forKey:@"objectType"];
    {
        NSDictionary* dic_renderRange = [[NSMutableDictionary alloc] init];
        [dic_renderRange setValue:@(_origin_renderRange.location) forKey:@"location"];
        [dic_renderRange setValue:@(_origin_renderRange.length) forKey:@"length"];
        [dic setValue:dic_renderRange forKey:@"renderRange"];
    }
    {
        NSDictionary* dic_contentSize = [[NSMutableDictionary alloc] init];
        [dic_contentSize setValue:@((float)_origin_contentSize.width) forKey:@"width"];
        [dic_contentSize setValue:@((float)_origin_contentSize.height) forKey:@"height"];
        [dic setValue:dic_contentSize forKey:@"contentSize"];
    }
    {
        NSDictionary* dic_position = [[NSMutableDictionary alloc] init];
        [dic_position setValue:@((float)_origin_position.x) forKey:@"x"];
        [dic_position setValue:@((float)_origin_position.y) forKey:@"y"];
        [dic setValue:dic_position forKey:@"position"];
    }
    {
        NSDictionary* dic_anchorPoint = [[NSMutableDictionary alloc] init];
        [dic_anchorPoint setValue:@((float)_origin_anchorPoint.x) forKey:@"x"];
        [dic_anchorPoint setValue:@((float)_origin_anchorPoint.y) forKey:@"y"];
        [dic setValue:dic_anchorPoint forKey:@"anchorPoint"];
    }
    {
        NSDictionary* dic_color4 = [[NSMutableDictionary alloc] init];
        [dic_color4 setValue:@(_origin_color4.r) forKey:@"r"];
        [dic_color4 setValue:@(_origin_color4.g) forKey:@"g"];
        [dic_color4 setValue:@(_origin_color4.b) forKey:@"b"];
        [dic_color4 setValue:@(_origin_color4.a) forKey:@"a"];
        [dic setValue:dic_color4 forKey:@"color4"];
    }
    [dic setValue:@(_origin_visible) forKey:@"visible"];
    [dic setValue:@(_origin_rotation) forKey:@"rotation"];
    [dic setValue:@(_origin_scaleX) forKey:@"scaleX"];
    [dic setValue:@(_origin_scaleY) forKey:@"scaleY"];
    [dic setValue:@(_origin_scaleZ) forKey:@"scaleZ"];
    [dic setValue:@(_origin_positionZ) forKey:@"positionZ"];
    [dic setValue:@(_origin_alpha) forKey:@"alpha"];
    return dic;
}

+ (void)deSerialize:(NSDictionary*)objDic output:(QGraphicNode*)outObj {
    {
        NSRange renderRange;
        NSDictionary* sub_dic = [objDic valueForKey:@"renderRange"];
        renderRange.location = [[sub_dic valueForKey:@"location"] intValue];
        renderRange.length = [[sub_dic valueForKey:@"length"] intValue];
        outObj.renderRange = renderRange;
    }
    {
        CGSize contentSize;
        NSDictionary* sub_dic = [objDic valueForKey:@"contentSize"];
        contentSize.width = [[sub_dic valueForKey:@"width"] floatValue];
        contentSize.height = [[sub_dic valueForKey:@"height"] floatValue];
        outObj.contentSize = contentSize;
    }
    {
        CGPoint position;
        NSDictionary* sub_dic = [objDic valueForKey:@"position"];
        position.x = [[sub_dic valueForKey:@"x"] floatValue];
        position.y = [[sub_dic valueForKey:@"y"] floatValue];
        outObj.position = position;
    }
    {
        CGPoint anchorPoint;
        NSDictionary* sub_dic = [objDic valueForKey:@"anchorPoint"];
        anchorPoint.x = [[sub_dic valueForKey:@"x"] floatValue];
        anchorPoint.y = [[sub_dic valueForKey:@"y"] floatValue];
        outObj.anchorPoint = anchorPoint;
    }
    {
        QColor4 color4;
        NSDictionary* sub_dic = [objDic valueForKey:@"color4"];
        color4.r = [[sub_dic valueForKey:@"r"] floatValue];
        color4.g = [[sub_dic valueForKey:@"g"] floatValue];
        color4.b = [[sub_dic valueForKey:@"b"] floatValue];
        color4.a = [[sub_dic valueForKey:@"a"] floatValue];
        outObj.color4 = color4;
    }
    outObj.visible = [[objDic valueForKey:@"visible"] boolValue];
    outObj.rotation = [[objDic valueForKey:@"rotation"] floatValue];
    outObj.scaleX = [[objDic valueForKey:@"scaleX"] floatValue];
    outObj.scaleY = [[objDic valueForKey:@"scaleY"] floatValue];
    outObj.scaleZ = [[objDic valueForKey:@"scaleZ"] floatValue];
    outObj.positionZ = [[objDic valueForKey:@"positionZ"] floatValue];
//    outObj.alpha = [[objDic valueForKey:@"alpha"] floatValue];
}

@end

@implementation QGraphicNode {
    NSString *_nodeName;
    GraphicCore::RenderNodeRef _graphicNode;
    NSMutableArray* _childrens;
    NSMutableArray* _animators;
}

@synthesize parent = _parent;

- (instancetype)initWithName:(NSString*)name
{
    if ((self = [super init]) != nil) {
        _graphicNode = GraphicCore::RenderNodeRef(new GraphicCore::RenderNode());
        _nodeName = [name copy];
        std::string s_name = std::string([name UTF8String]);
        _graphicNode->setName(s_name);
        _childrens = [[NSMutableArray alloc] init];
        _animators = [[NSMutableArray alloc] init];
        _parent = nil;
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
        _parent = nil;
    }
    return self;
}

- (GraphicCore::RenderNodeRef)native {
    return _graphicNode;
}
#pragma mark - private function //

- (void)setParent:(QGraphicNode *)parent {
    _parent = parent;
}

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
        [childNode setParent:self];
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

- (bool)addChildNodeDirect:(QGraphicNode*)childNode {
    if (childNode != nil && ![_childrens containsObject:childNode]) {
        [_childrens addObject:childNode];
        [childNode setParent:self];
        return true;
    }
    return false;
}

- (bool)removeChildNodeDirect:(QGraphicNode*)childNode {
    if([_childrens containsObject:childNode])
    {
        [childNode setParent:nil];
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
    super.origin_renderRange = renderRange;
    _graphicNode->setRange(Range<int64_t>(renderRange.location, renderRange.location + renderRange.length));
}

- (bool)visible{
    return _graphicNode->isVisible();
}
- (void)setVisible:(bool)visible{
    super.origin_visible = visible;
    _graphicNode->setVisible(visible);
}
- (float)rotation{
    return _graphicNode->getRotation();
}
- (void)setRotation:(float)rotation{
    super.origin_rotation = rotation;
    _graphicNode->setRotation(rotation);
}
- (float)scaleX{
    return _graphicNode->getScaleX();
}
- (void)setScaleX:(float)scaleX{
    super.origin_scaleX = scaleX;
    _graphicNode->setScaleX(scaleX);
}
- (float)scaleY{
    return _graphicNode->getScaleY();
}
- (void)setScaleY:(float)scaleY{
    super.origin_scaleY = scaleY;
    _graphicNode->setScaleY(scaleY);
}
- (float)scaleZ{
    return _graphicNode->getScaleZ();
}
- (void)setScaleZ:(float)scaleZ{
    super.origin_scaleZ = scaleZ;
    _graphicNode->setScaleZ(scaleZ);
}

- (CGSize)contentSize{
    GraphicCore::Size sz = _graphicNode->getContentSize();
    return CGSizeMake(sz.width,sz.height);
}
- (void)setContentSize:(CGSize)contentSize{
    super.origin_contentSize = contentSize;
    GraphicCore::Size sz(contentSize.width,contentSize.height);
    _graphicNode->setContentSize(sz);
}

- (CGPoint)position{
    GraphicCore::Vec2 pos = _graphicNode->getPosition();
    return CGPointMake(pos.x, pos.y);
}
- (void)setPosition:(CGPoint)position{
    super.origin_position = position;
    GraphicCore::Vec2 pos(position.x,position.y);
    _graphicNode->setPosition(pos);
}
- (float)positionZ{
    return _graphicNode->getPositionZ();
}
- (void)setPositionZ:(float)positionZ{
    super.origin_positionZ = positionZ;
    _graphicNode->setPositionZ(positionZ);
}

- (CGPoint)anchorPoint{
    GraphicCore::Vec2 anchor = _graphicNode->getAnchorPoint();
    return CGPointMake(anchor.x, anchor.y);
}
- (void)setAnchorPoint:(CGPoint)anchorPoint{
    super.origin_anchorPoint = anchorPoint;
    GraphicCore::Vec2 anchor(anchorPoint.x,anchorPoint.y);
    _graphicNode->setAnchorPoint(anchor);
}

- (QColor4)color4{
    GraphicCore::Color4F color = _graphicNode->getColor();
    return QColorMaker(color.r, color.g, color.b, color.a);
}
- (void)setColor4:(QColor4)color4{
    super.origin_color4 = color4;
    GraphicCore::Color4F color(color4.r,color4.g,color4.b,color4.a);
    _graphicNode->setColor(color);
}

- (float)alpha{
    GraphicCore::Color4F color = _graphicNode->getColor();
    return color.a;
}
- (void)setAlpha:(float)alpha{
    super.origin_alpha = alpha;
    GraphicCore::Color4F color = _graphicNode->getColor();
    color.a = alpha;
    _graphicNode->setColor(color);
}

@end
