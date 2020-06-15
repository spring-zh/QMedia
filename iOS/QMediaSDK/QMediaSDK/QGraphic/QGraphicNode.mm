//
//  QGraphicNode.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QGraphicNode_internal.h"
#import "QCombiner_internal.h"


QColor4 QColorMake(float r, float g, float b, float a) {
    QColor4 color4;
    color4.r = r; color4.g = g; color4.b = b; color4.a = a;
    return color4;
}

//blend
unsigned Blend_ZERO = GL_ZERO;
unsigned Blend_ONE = GL_ONE;
unsigned Blend_SRC_COLOR = GL_SRC_COLOR;
unsigned Blend_ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR;
unsigned Blend_SRC_ALPHA = GL_SRC_ALPHA;
unsigned Blend_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA;
unsigned Blend_DST_ALPHA = GL_DST_ALPHA;
unsigned Blend_ONE_MINUS_DST_ALPHA = GL_ONE_MINUS_DST_ALPHA;

unsigned Blend_DST_COLOR = GL_DST_COLOR;
unsigned Blend_ONE_MINUS_DST_COLOR = GL_ONE_MINUS_DST_COLOR;
unsigned Blend_SRC_ALPHA_SATURATE = GL_SRC_ALPHA_SATURATE;

QBlendFunc QBlendFuncMake(unsigned src, unsigned dst) {
    QBlendFunc blendFunc = {src , dst};
    return blendFunc;
}

QBlendFunc QBlendDisable = {GL_ONE, GL_ZERO};

using GraphicCore::AnimaNode;

@interface QNodeAnimator(internal)
@property (nonatomic, readonly) AnimaNode::Animator* native;
@end

#pragma mark QGraphicNode

@implementation QGraphicNode {
    NSString *_uid;
    NSString *_nodeName;
    GraphicCore::RenderNodeRef _graphicNode;
    NSMutableArray* _childrens;
    NSMutableArray* _animators;
    __weak QCombiner* _combiner;
}

@synthesize parent = _parent;

- (instancetype)init{
    if ((self = [super init]) != nil) {
        self.origin_renderRange = NSMakeRange(0, 0);
        self.origin_visible = false;
//        self.origin_rotation = 0;
        self.origin_rotation3d = QVectorV3(0, 0, 0);
        self.origin_scaleX = 1.0f;
        self.origin_scaleY = 1.0f;
        self.origin_scaleZ = 1.0f;
        self.origin_contentSize = CGSizeMake(0, 0);
        self.origin_position = CGPointMake(0, 0);
        self.origin_positionZ = 0;
        self.origin_anchorPoint = CGPointMake(0, 0);
        self.origin_color4 = QColorMake(1, 1, 1, 1);
//        self.origin_alpha = 1.0f;
        self.origin_visible = true;
        self.origin_crop = QVectorV4(0,0,0,0);
        self.origin_blendFunc = QBlendDisable;
    }
    return self;
}

- (instancetype)initWithName:(NSString*)name combiner:(QCombiner*)combiner
{
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initWithName:name combiner:combiner uid:(__bridge_transfer NSString *)uuid];
}

- (instancetype)initWithName:(NSString*)name combiner:(QCombiner*)combiner uid:(NSString*)uid
{
    if ((self = [self init]) != nil) {
        _uid = [uid copy];
        _combiner = combiner;
        _graphicNode = GraphicCore::RenderNodeRef(new GraphicCore::RenderNode());
        _nodeName = [name copy];
        std::string s_name = std::string([name UTF8String]);
        _graphicNode->setName(s_name);
        _childrens = [[NSMutableArray alloc] init];
        _animators = [[NSMutableArray alloc] init];
        _parent = nil;
        
        [_combiner addGraphicNodeIndex:self];
    }
    return self;
}

#pragma mark - private function
- (instancetype)initWithNode:(GraphicCore::RenderNodeRef)graphicNode combiner:(QCombiner*)combiner
{
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initWithNode:graphicNode combiner:combiner uid:(__bridge_transfer NSString *)uuid];
}

- (instancetype)initWithNode:(GraphicCore::RenderNodeRef)graphicNode combiner:(QCombiner*)combiner  uid:(NSString*)uid
{
    if ((self = [self init]) != nil) {
        _uid = uid;
        _combiner = combiner;
        _graphicNode = graphicNode;
        if (_graphicNode == nullptr) {
            return nil;
        }
        _nodeName = [NSString stringWithUTF8String:graphicNode->getName().c_str()];
        _childrens = [[NSMutableArray alloc] init];
        _animators = [[NSMutableArray alloc] init];
        _parent = nil;
        
        [_combiner addGraphicNodeIndex:self];
    }
    return self;
}

- (NSString*)uid {
    return _uid;
}

- (GraphicCore::RenderNodeRef)native {
    return _graphicNode;
}
#pragma mark - private function //

- (void)setName:(NSString *)name {
    _nodeName = [name copy];
    std::string s_name = std::string([name UTF8String]);
    _graphicNode->setName(s_name);
}

- (void)setParent:(QGraphicNode *)parent {
    _parent = parent;
}

- (NSArray*)childrens{
    return _childrens;
}

- (bool)addChildNode:(QGraphicNode*)childNode
{
    if (childNode != nil && ![_childrens containsObject:childNode]) {
        [_childrens addObject:childNode];
        [childNode setParent:self];
        [_combiner attachRenderNode:childNode parent:self];
        return true;
    }
    return false;
}
- (bool)removeChildNode:(QGraphicNode*)childNode
{
    if([_childrens containsObject:childNode])
    {
        [childNode setParent:nil];
        [_childrens removeObject:childNode];
        [_combiner detachRenderNode:childNode];
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
    return _graphicNode->addAnimator(animator.native);
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

- (void)copyFrom:(QGraphicNode*)fromNode {
    _uid = fromNode.uid;
    [self setName:fromNode.name];
    self.position = fromNode.origin_position;
    self.positionZ = fromNode.origin_positionZ;
    self.contentSize = fromNode.origin_contentSize;
    self.anchorPoint = fromNode.origin_anchorPoint;
    self.color4 = fromNode.origin_color4;
    self.rotation3d = fromNode.origin_rotation3d;
    self.scaleX = fromNode.origin_scaleX;
    self.scaleY = fromNode.origin_scaleY;
    self.scaleZ = fromNode.origin_scaleZ;
    self.visible = fromNode.origin_visible;
    self.renderRange = fromNode.renderRange;
    self.crop = fromNode.origin_crop;
    self.blendFunc = fromNode.origin_blendFunc;
    
    [_animators removeAllObjects];
    for (QNodeAnimator* animator in fromNode.animators) {
        [self addAnimator:[[QNodeAnimator alloc] initWith:animator.property range:animator.timeRang begin:animator.beginPoint end:animator.endPoint functype:animator.functionType repleat:animator.repleat]];
    }
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
    self.origin_renderRange = renderRange;
    _graphicNode->setRange(Range<int64_t>(renderRange.location, renderRange.location + renderRange.length));
}

- (bool)visible{
    return _graphicNode->isVisible();
}
- (void)setVisible:(bool)visible{
    self.origin_visible = visible;
    _graphicNode->setVisible(visible);
}
- (float)rotation{
    return _graphicNode->getRotation();
}
- (void)setRotation:(float)rotation{
    QVector vertor3 = self.origin_rotation3d;
    vertor3.v2 = rotation;
    self.origin_rotation3d = vertor3;
    _graphicNode->setRotation(rotation);
}
- (QVector)rotation3d {
    auto rotation3d = _graphicNode->getRotation3D();
    return QVectorV3(rotation3d.x, rotation3d.y, rotation3d.z);
}
- (void)setRotation3d:(QVector)rotation3d {
    self.origin_rotation3d = rotation3d;
    _graphicNode->setRotation3D(GraphicCore::Vec3(rotation3d.v0, rotation3d.v1, rotation3d.v2));
}
- (float)scaleX{
    return _graphicNode->getScaleX();
}
- (void)setScaleX:(float)scaleX{
    self.origin_scaleX = scaleX;
    _graphicNode->setScaleX(scaleX);
}
- (float)scaleY{
    return _graphicNode->getScaleY();
}
- (void)setScaleY:(float)scaleY{
    self.origin_scaleY = scaleY;
    _graphicNode->setScaleY(scaleY);
}
- (float)scaleZ{
    return _graphicNode->getScaleZ();
}
- (void)setScaleZ:(float)scaleZ{
    self.origin_scaleZ = scaleZ;
    _graphicNode->setScaleZ(scaleZ);
}

- (CGSize)contentSize{
    GraphicCore::Size sz = _graphicNode->getContentSize();
    return CGSizeMake(sz.width,sz.height);
}
- (void)setContentSize:(CGSize)contentSize{
    self.origin_contentSize = contentSize;
    GraphicCore::Size sz(contentSize.width,contentSize.height);
    _graphicNode->setContentSize(sz);
}

- (CGPoint)position{
    GraphicCore::Vec2 pos = _graphicNode->getPosition();
    return CGPointMake(pos.x, pos.y);
}
- (void)setPosition:(CGPoint)position{
    self.origin_position = position;
    GraphicCore::Vec2 pos(position.x,position.y);
    _graphicNode->setPosition(pos);
}
- (float)positionZ{
    return _graphicNode->getPositionZ();
}
- (void)setPositionZ:(float)positionZ{
    self.origin_positionZ = positionZ;
    _graphicNode->setPositionZ(positionZ);
}

- (CGPoint)anchorPoint{
    GraphicCore::Vec2 anchor = _graphicNode->getAnchorPoint();
    return CGPointMake(anchor.x, anchor.y);
}
- (void)setAnchorPoint:(CGPoint)anchorPoint{
    self.origin_anchorPoint = anchorPoint;
    GraphicCore::Vec2 anchor(anchorPoint.x,anchorPoint.y);
    _graphicNode->setAnchorPoint(anchor);
}

- (QColor4)color4{
    GraphicCore::Color4F color = _graphicNode->getColor();
    return QColorMake(color.r, color.g, color.b, color.a);
}
- (void)setColor4:(QColor4)color4{
    self.origin_color4 = color4;
    GraphicCore::Color4F color(color4.r,color4.g,color4.b,color4.a);
    _graphicNode->setColor(color);
}

- (float)alpha{
    GraphicCore::Color4F color = _graphicNode->getColor();
    return color.a;
}
- (void)setAlpha:(float)alpha{
    QColor4 qcolor = self.origin_color4;
    qcolor.a = alpha;
    self.origin_color4 = qcolor;
    
    GraphicCore::Color4F color = {qcolor.r, qcolor.g, qcolor.b, qcolor.a};
    _graphicNode->setColor(color);
}

- (QVector)crop {
    GraphicCore::Rect crop = _graphicNode->getCrop();
    return QVectorV4(crop.getMaxX(), crop.getMaxY(), crop.getMaxX(), crop.getMaxY());
}
- (void)setCrop:(QVector)crop {
    self.origin_crop = crop;
    _graphicNode->setCrop(GraphicCore::Rect(crop.v0,crop.v1, crop.v2 - crop.v0, crop.v3 - crop.v1));
}

- (QBlendFunc)blendFunc {
    return QBlendFuncMake(_graphicNode->getBlendFunc().src, _graphicNode->getBlendFunc().dst);
}
- (void)setBlendFunc:(QBlendFunc)blendFunc {
    self.origin_blendFunc = blendFunc;
    GraphicCore::BlendFunc blend_func = {blendFunc.src, blendFunc.dst};
    _graphicNode->setBlendFunc(blend_func);
}

@end
