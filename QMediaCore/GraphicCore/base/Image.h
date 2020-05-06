//
//  Image.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_IMAGE_H
#define GRAPHICCORE_IMAGE_H

#include <memory>
#include "base/Macros.h"

GRAPHICCORE_BEGIN

class Image {
public:
    enum Format : int{
        RGBA = 0,
        RGB,
        LUMINANCE,
        LUMINANCE_ALPHA,
        UNKNOW
    };
    
    static Image* createFromFile(FILE *filep);
    static Image* createEmpty(Format format, int width, int height);
    
    virtual ~Image();
    
    virtual void *getBuffer() const { return _buffer; }
    virtual Format getFormat() const { return _format; }
    virtual int width() const { return _width;}
    virtual int height() const { return _height;}
    
protected:
    Image();
    
    void *_buffer;
    Format _format;
    int _width;
    int _height;
    
    int _create_mode;
};

using ImageRef = std::shared_ptr<Image>;

GRAPHICCORE_END

#endif /* GRAPHICCORE_IMAGE_H */
