//
//  FilterFactory.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "FilterFactory.h"
#include "PolarPixellateFilter.h"

GRAPHICCORE_BEGIN

TextureFilter* TextureFilterFactory::createFilter(FilterType filterType) {
//    auto iter = _filterCache.find(filterType);
//    if (iter == _filterCache.end()) {
//        //TODO: create and add new Filter
//    }
//
//    return _filterCache[filterType];
    return new PolarPixellateFilter();
}

GRAPHICCORE_END
