//
//  QNodeAnimator.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QNodeAnimator.h"
#include "GraphicCore/GcRenderNode.h"

using GraphicCore::AnimaNode;

AnimaNode::AnimatorRef QNodeAnimatorToCPP(QNodeAnimator* animator){
    
    Range<int64_t> range = {static_cast<int64_t>(animator.timeRang.location), static_cast<int64_t>(animator.timeRang.length + animator.timeRang.length)};
    AnimaNode::AnimatorData bData(GraphicCore::Vec4(animator.beginPoint.v0,animator.beginPoint.v1,animator.beginPoint.v2,animator.beginPoint.v3));
    AnimaNode::AnimatorData eData(GraphicCore::Vec4(animator.endPoint.v0,animator.endPoint.v1,animator.endPoint.v2,animator.endPoint.v3));
    return AnimaNode::AnimatorRef(new AnimaNode::Animator([animator.property UTF8String], range, bData, eData, (GraphicCore::EaseFunctionType)animator.functionType, animator.repleat ,[animator.name UTF8String]));
}

#pragma mark QNodeAnimator
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
