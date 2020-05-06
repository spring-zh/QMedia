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

@interface QLayer : QGraphicNode

- (instancetype)initWithSize:(CGSize)size name:(NSString*)name;
@property (nonatomic) QColor4 bkColor;

@end
