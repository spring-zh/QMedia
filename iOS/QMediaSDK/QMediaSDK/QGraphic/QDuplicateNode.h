//
//  QDuplicateNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

/**
 * graphic layer which owner framebuffer and scene
 */

#import "QGraphicNode.h"

@interface QDuplicateNode : QGraphicNode

- (instancetype)initFromNode:(QGraphicNode*)nodeFrom;
@property (nonatomic) QColor4 bkColor;

@end
