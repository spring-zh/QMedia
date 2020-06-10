//
//  QImageNode.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

/**
 * load and draw one image
 */

#import "QGraphicNode.h"

@class QCombiner;

@interface QImageNode : QGraphicNode

- (instancetype)initWithPath:(NSString*)filePath combiner:(QCombiner*)combiner;
- (instancetype)initWithPath:(NSString*)filePath combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) NSString* filePath;
@property (nonatomic, readonly) CGSize imgSize;

@end
