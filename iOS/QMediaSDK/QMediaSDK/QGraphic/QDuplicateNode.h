//
//  QDuplicateNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//
#import "QGraphicNode.h"

/**
 * a graphic node which duplicate other graphic node
*/
@interface QDuplicateNode : QGraphicNode

- (instancetype)initFromNode:(QGraphicNode*)nodeFrom;
@property (nonatomic, readonly) QGraphicNode* nodeFrom;
@property (nonatomic) QColor4 bkColor;

@end
