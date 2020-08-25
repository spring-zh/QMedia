//
//  QGraphicNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QNodeAnimator.h"
#import "QTrack.h"
#import "QEffect.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct QColor4 {
    float r, g, b, a;
}QColor4;
QColor4 QColorMake(float r, float g, float b, float a);

// for blend
extern unsigned Blend_ZERO;
extern unsigned Blend_ONE;
extern unsigned Blend_SRC_COLOR;
extern unsigned Blend_ONE_MINUS_SRC_COLOR;
extern unsigned Blend_SRC_ALPHA;
extern unsigned Blend_ONE_MINUS_SRC_ALPHA;
extern unsigned Blend_DST_ALPHA;
extern unsigned Blend_ONE_MINUS_DST_ALPHA;

extern unsigned Blend_DST_COLOR;
extern unsigned Blend_ONE_MINUS_DST_COLOR;
extern unsigned Blend_SRC_ALPHA_SATURATE;

typedef struct QBlendFunc {
    unsigned src, dst;
}QBlendFunc;

extern QBlendFunc QBlendDisable;
QBlendFunc QBlendFuncMake(unsigned src, unsigned dst);

#ifdef __cplusplus
}
#endif

@class QCombiner;

// QGraphicNode
@interface QGraphicNode : NSObject <QTrack>

- (instancetype)initWithName:(NSString*)name combiner:(QCombiner*)combiner;
- (instancetype)initWithName:(NSString*)name combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic) NSString* name;

@property (nonatomic, weak, readonly) QGraphicNode* parent;
//for children nodes
@property (nonatomic, readonly) NSArray* childrens;

- (bool)addChildNode:(QGraphicNode*)childNode;
- (bool)removeChildNode:(QGraphicNode*)childNode;
- (void)clearAllChildrens;

//set childnode display to top
- (void)topChildNode:(QGraphicNode*)childNode;

//remove self form combiner index
- (void)releaseIndex;

//for animator
@property (nonatomic, readonly) NSArray<QNodeAnimator*>* animators;
- (bool)addAnimator:(QNodeAnimator*)animator;
- (bool)removeAnimator:(QNodeAnimator*)animator;
- (void)clearAllAnimators;

//effects
@property (nonatomic, readonly) NSArray<QEffect*>* effects;
- (void)addEffect:(QEffect*)effect;
- (void)removeEffect:(QEffect*)effect;
- (void)removeAllEffect;

- (void)copyFrom:(QGraphicNode*)fromNode;

//QTrack
@property (nonatomic, readonly) NSString* uid;
@property (nonatomic, readonly) NSString* displayName;
@property (nonatomic) QTimeRange sourceRange;
@property (nonatomic) QTimeRange displayRange;

//transform propertys setting
@property (nonatomic) bool visible;
@property (nonatomic) float rotation;
@property (nonatomic) QVector rotation3d;
@property (nonatomic) float scaleX;
@property (nonatomic) float scaleY;
@property (nonatomic) float scaleZ;

//display size
@property (nonatomic) CGSize contentSize;
@property (nonatomic) CGPoint position;
@property (nonatomic) float positionZ;

/**
 * anchorPoint for rotation and scale
 * set (0.5,0.5) to the center of current object, default value is (0,0)
 */
@property (nonatomic) CGPoint anchorPoint;

//the mix color
@property (nonatomic) QColor4 color4;
@property (nonatomic) float alpha;

//texture crop (0.0 ~ 1.0)
@property (nonatomic) QVector crop;

@property (nonatomic) QBlendFunc blendFunc;

@end
