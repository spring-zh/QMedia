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

@class QGraphicNode;

/**
 * the QGraphicNode 's Serialize propretys
 */
@interface QGraphicNode_Serialize : NSObject <Serializable>
@property (nonatomic, assign) NSRange origin_renderRange;
@property (nonatomic, assign) bool origin_visible;
@property (nonatomic, assign) float origin_rotation;
@property (nonatomic, assign) float origin_scaleX;
@property (nonatomic, assign) float origin_scaleY;
@property (nonatomic, assign) float origin_scaleZ;

@property (nonatomic, assign) CGSize origin_contentSize;
@property (nonatomic, assign) CGPoint origin_position;
@property (nonatomic, assign) float origin_positionZ;
@property (nonatomic, assign) CGPoint origin_anchorPoint;
@property (nonatomic, assign) QColor4 origin_color4;
@property (nonatomic, assign) float origin_alpha;

- (NSDictionary*)serialize;
+ (void)deSerialize:(NSDictionary*)objDic output:(QGraphicNode*)outObj;

@end

// QGraphicNode
@interface QGraphicNode : QGraphicNode_Serialize

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
