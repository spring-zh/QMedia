//
//  QLayer.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

/**
 * graphic layer which owner framebuffer and scene
 */

#import "QGraphicNode.h"
#import "QEffect.h"

@class QCombiner;

typedef NS_ENUM(NSInteger, QFlipMode) {
    QFlipNONE = 0,
    QFlipH = 1,
    QFlipV = 2
};

@interface QLayer : QGraphicNode

- (instancetype)initWithSize:(CGSize)size combiner:(QCombiner*)combiner;
- (instancetype)initWithSize:(CGSize)size combiner:(QCombiner*)combiner uid:(NSString*)uid;

@property (nonatomic, readonly) CGSize layerSize;
@property (nonatomic) QColor4 bkColor;
@property (nonatomic) bool enable3d;
@property (nonatomic) QFlipMode flipMode;

@end
