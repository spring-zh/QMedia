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

- (instancetype)initWithSize:(CGSize)size name:(NSString*)name
{
    _layer = GraphicCore::LayerRef(new GraphicCore::Layer(GraphicCore::Size(size.width,size.height)));
//    _layer->setContentSize(GraphicCore::Size(size.width, size.height));
    std::string s_name = std::string([name UTF8String]);
    _layer->setName(s_name);
    _bkColor = QColorMaker(0, 0, 0, 0);
    return (self = [super initWithNode:_layer]);
}

- (void)dealloc {
    NSLog(@"QLayer dealloc");
}

- (instancetype)initWithLayer:(GraphicCore::LayerRef)layer
{
    if ((self = [super initWithNode:layer]) != nil) {
        _layer = layer;
    }
    return self;
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
