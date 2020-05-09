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

- (NSDictionary*)serialize {
    NSDictionary* dic = [[NSMutableDictionary alloc] init];
    [dic setValue:NSStringFromClass(self.class) forKey:@"objectType"];
    [dic setValue:_property forKey:@"property"];
    {
        NSDictionary* dic_timeRang = [[NSMutableDictionary alloc] init];
        [dic_timeRang setValue:@(_timeRang.location) forKey:@"location"];
        [dic_timeRang setValue:@(_timeRang.length) forKey:@"length"];
        [dic setValue:dic_timeRang forKey:@"timeRang"];
    }
    {
        NSDictionary* dic_beginPoint = [[NSMutableDictionary alloc] init];
        [dic_beginPoint setValue:@(_beginPoint.v0) forKey:@"v0"];
        [dic_beginPoint setValue:@(_beginPoint.v1) forKey:@"v1"];
        [dic_beginPoint setValue:@(_beginPoint.v2) forKey:@"v2"];
        [dic_beginPoint setValue:@(_beginPoint.v3) forKey:@"v3"];
        [dic setValue:dic_beginPoint forKey:@"beginPoint"];
    }
    {
        NSDictionary* dic_endPoint = [[NSMutableDictionary alloc] init];
        [dic_endPoint setValue:@(_endPoint.v0) forKey:@"v0"];
        [dic_endPoint setValue:@(_endPoint.v1) forKey:@"v1"];
        [dic_endPoint setValue:@(_endPoint.v2) forKey:@"v2"];
        [dic_endPoint setValue:@(_endPoint.v3) forKey:@"v3"];
        [dic setValue:dic_endPoint forKey:@"endPoint"];
    }
    [dic setValue:_name forKey:@"name"];
    [dic setValue:@(_functionType) forKey:@"functionType"];
    [dic setValue:@(_repleat) forKey:@"repleat"];
    return dic;
}

+ (QNodeAnimator*)deSerialize:(NSDictionary*)objDic{
    if(![NSClassFromString([objDic valueForKey:@"objectType"]) isEqual:QNodeAnimator.class])
        return nil;
//    QNodeAnimator* nodeAnimator = [[QNodeAnimator alloc] init];
    NSString* name = [objDic valueForKey:@"name"];
    NSString* property = [objDic valueForKey:@"property"];
    NSRange timeRang;
    {
        NSDictionary* timeRang_dic = [objDic valueForKey:@"timeRang"];
        timeRang.length = [[timeRang_dic valueForKey:@"length"] integerValue];
        timeRang.location = [[timeRang_dic valueForKey:@"location"] integerValue];
    }
    QVector beginPoint;
    {
        NSDictionary* beginPoint_dic = [objDic valueForKey:@"beginPoint"];
        beginPoint.v0 = [[beginPoint_dic valueForKey:@"v0"] floatValue];
        beginPoint.v1 = [[beginPoint_dic valueForKey:@"v1"] floatValue];
        beginPoint.v2 = [[beginPoint_dic valueForKey:@"v2"] floatValue];
        beginPoint.v3 = [[beginPoint_dic valueForKey:@"v3"] floatValue];
    }
    QVector endPoint;
    {
        NSDictionary* endPoint_dic = [objDic valueForKey:@"endPoint"];
        endPoint.v0 = [[endPoint_dic valueForKey:@"v0"] floatValue];
        endPoint.v1 = [[endPoint_dic valueForKey:@"v1"] floatValue];
        endPoint.v2 = [[endPoint_dic valueForKey:@"v2"] floatValue];
        endPoint.v3 = [[endPoint_dic valueForKey:@"v3"] floatValue];
    }
    QEaseFunction functionType = (QEaseFunction)[[objDic valueForKey:@"functionType"] intValue];
    bool repleat = [[objDic valueForKey:@"repleat"] boolValue];
    return [[QNodeAnimator alloc] initWith:property range:timeRang begin:beginPoint end:endPoint functype:functionType repleat:repleat name:name];
}

@end
