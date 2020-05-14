//
//  QGraphicNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QNodeAnimator.h"

#ifdef __cplusplus
extern "C" {
#endif

//typedef QVector QColor4;
//#define QColorMaker QVectorV4

typedef struct QColor4 {
    float r, g, b, a;
}QColor4;
QColor4 QColorMaker(float r, float g, float b, float a);

#ifdef __cplusplus
}
#endif

// QGraphicNode
@interface QGraphicNode : NSObject

- (instancetype)initWithName:(NSString*)name;
@property (nonatomic, readonly) NSString* name;

@property (nonatomic, weak, readonly) QGraphicNode* parent;
//for children nodes
@property (nonatomic, readonly) NSArray* childrens;
/**
 * use QCombiner attachRenderNode/detachRenderNode to instead of these
 */
- (bool)addChildNode:(QGraphicNode*)childNode DEPRECATED_ATTRIBUTE;
- (bool)removeChildNode:(QGraphicNode*)childNode DEPRECATED_ATTRIBUTE;
- (void)clearAllChildrens;

//for animator
@property (nonatomic, readonly) NSArray<QNodeAnimator*>* animators;
- (bool)addAnimator:(QNodeAnimator*)animator;
- (bool)removeAnimator:(QNodeAnimator*)animator;
- (void)clearAllAnimators;

@property (nonatomic) NSRange renderRange;

//transform propertys setting
@property (nonatomic) bool visible;
@property (nonatomic) float rotation;
@property (nonatomic) QVector rotation3d;
@property (nonatomic) float scaleX;
@property (nonatomic) float scaleY;
@property (nonatomic) float scaleZ;

@property (nonatomic) CGSize contentSize;
@property (nonatomic) CGPoint position;
@property (nonatomic) float positionZ;

/**
 * anchorPoint for rotation and scale
 * set (0.5,0.5) to the center of current object, default value is (0,0)
 */
@property (nonatomic) CGPoint anchorPoint;

//the mix color, only effective for XMLayer
@property (nonatomic) QColor4 color4;
@property (nonatomic) float alpha;

@end
