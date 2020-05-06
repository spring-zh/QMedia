//
//  GcFilter.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_FILTER_H
#define GRAPHICCORE_FILTER_H

#include <memory>
#include "base/Macros.h"

GRAPHICCORE_BEGIN

class Filter {
public:
    Filter();
    virtual ~Filter();
    
};

using FilterRef = std::shared_ptr<Filter>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_FILTER_H */
