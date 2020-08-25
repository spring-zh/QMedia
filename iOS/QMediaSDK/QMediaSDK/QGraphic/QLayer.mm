//
//  QLayer.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GraphicCore/GcLayer.h"
#import "QLayer.h"
#import "QCombiner_internal.h"
#import "QGraphicNode_internal.h"
#import "QEffect_internal.h"

using namespace GraphicCore;

@implementation QLayer {
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
    return self = [super initWithNode:_layer combiner:combiner uid:uid];
}

- (instancetype)initWithNative:(GraphicCore::LayerRef)layer combiner:(QCombiner*)combiner uid:(NSString*)uid
{
    _layer = layer;
    return self = [super initWithNode:_layer combiner:combiner uid:uid];
}

- (void)dealloc {
    NSLog(@"QLayer dealloc");
}

- (CGSize)layerSize {
    return CGSizeMake(_layer->getLayerSize().width, _layer->getLayerSize().height);
}

- (QColor4)bkColor{
    GraphicCore::Color4F color = _layer->getBKColor();
    return QColorMake(color.r, color.g, color.b, color.a);
}
- (void)setBkColor:(QColor4)bkColor{
    GraphicCore::Color4F color(bkColor.r,bkColor.g,bkColor.b,bkColor.a);
    _layer->setBKColor(color);
}

- (bool)enable3d{
    return _layer->getEnable3d();
}

- (void)setEnable3d:(bool)enable3d {
    _layer->setEnable3d(enable3d);
}

- (QFlipMode)flipMode {
    return (QFlipMode)_layer->getFlipMode();
}

- (void)setFlipMode:(QFlipMode)flipMode {
    _layer->setFlipMode((Drawable2D::FlipMode)flipMode);
}

@end
