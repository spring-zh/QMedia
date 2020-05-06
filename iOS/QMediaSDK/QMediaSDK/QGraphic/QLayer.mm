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
    GraphicCore::LayerRef _layer;
}

- (instancetype)initWithSize:(CGSize)size name:(NSString*)name
{
    _layer = GraphicCore::LayerRef(new GraphicCore::Layer(GraphicCore::Size(size.width,size.height)));
//    _layer->setContentSize(GraphicCore::Size(size.width, size.height));
    std::string s_name = std::string([name UTF8String]);
    _layer->setName(s_name);
    return (self = [super initWithNode:_layer]);
}

- (instancetype)initWithLayer:(GraphicCore::LayerRef)layer
{
    if ((self = [super initWithNode:layer]) != nil) {
        _layer = layer;
    }
    return self;
}

- (QColor4)bkColor{
    GraphicCore::Color4F color = _layer->getBKColor();
    return QColorMaker(color.r, color.g, color.g, color.b);
}
- (void)setBkColor:(QColor4)bkColor{
    GraphicCore::Color4F color(bkColor.r,bkColor.g,bkColor.b,bkColor.a);
    _layer->setBKColor(color);
}

@end
