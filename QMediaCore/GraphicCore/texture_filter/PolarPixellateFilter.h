//
//  PolarPixellateFilter.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_POLARPIXELLATE_FILTER_H
#define GRAPHICCORE_POLARPIXELLATE_FILTER_H

#include "FilterFactory.h"
#include "GraphicCore/math/CCGeometry.h"

GRAPHICCORE_BEGIN

class PolarPixellateFilter : public TextureFilter{
public:
    PolarPixellateFilter();
    virtual ~PolarPixellateFilter() {}
    
    void setCenter(const Vec2& center) { _center = center; }
    void setPixelSize(const Vec2& pixelSize) { _pixelSize = pixelSize; }
    
    // call on gl thread
    bool create() override;
    void drawFilter(int64_t duration, const Texture2D *srcTexture, int flags) override;
    void release() override;
    
private:
    Vec2 _center;
    Vec2 _pixelSize;
};

GRAPHICCORE_END

#endif /* GRAPHICCORE_POLARPIXELLATE_FILTER_H */
