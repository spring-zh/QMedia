//
//  QLayer.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QLayer.h"
#import "QGraphicNode_internal.h"
#include "GraphicCore/GcLayer.h"

@implementation QLayer {
    QColor4 _bkColor;
    GraphicCore::LayerRef _layer;
}

- (instancetype)initWithSize:(CGSize)size combiner:(QCombiner*)combiner
{
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initWithSize:size combiner:combiner uid:(__bridge_transfer NSString *)uuid];
}

- (instancetype)initWithSize:(CGSize)size combiner:(QCombiner*)combiner uid:(NSString*)uid
{
    _layer = GraphicCore::LayerRef(new GraphicCore::Layer(GraphicCore::Size(size.width,size.height)));
    _bkColor = QColorMaker(0, 0, 0, 0);
    return (self = [super initWithNode:_layer combiner:combiner uid:uid]);
}

- (void)dealloc {
    NSLog(@"QLayer dealloc");
}

- (CGSize)layerSize {
    return CGSizeMake(_layer->getLayerSize().width, _layer->getLayerSize().height);
}

- (QColor4)bkColor{
    GraphicCore::Color4F color = _layer->getBKColor();
    return QColorMaker(color.r, color.g, color.b, color.a);
}
- (void)setBkColor:(QColor4)bkColor{
    _bkColor = bkColor;
    GraphicCore::Color4F color(bkColor.r,bkColor.g,bkColor.b,bkColor.a);
    _layer->setBKColor(color);
}

@end
