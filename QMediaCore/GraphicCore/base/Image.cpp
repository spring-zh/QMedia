//
//  Image.cpp
//  QMediaCore
//
//  Created by spring on 2019/2/23.
//  Copyright © 2019 QMedia. All rights reserved.
//

#include "Image.h"

/////////////////////////////////////////////////////////////////////////////////////
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
//HAVE_NEON is defined in Android.mk !
#ifdef HAVE_NEON
#define STBI_NEON
#endif // !HAVE_NEON
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

GRAPHICCORE_BEGIN

Image::Image():
_buffer(NULL),
_format(UNKNOW),
_width(0),
_height(0),
_create_mode(0)
{
    
}

Image::~Image()
{
    if (_buffer) {
        if (_create_mode == 1) {
            stbi_image_free(_buffer);
        }else {
            free(_buffer);
        }
    }
    _buffer = NULL;
}

Image* Image::createFromFile(FILE *filep)
{
    Image* image = nullptr;
    int w,h,n;
    stbi_uc *idata = stbi_load_from_file(filep, &w, &h, &n,0);
    if (idata == NULL) {
        return nullptr;
    }
    image = new /*(std::nothrow)*/ Image;
    image->_width = w;
    image->_height = h;
    switch (n) {
        case STBI_grey:
            image->_format = LUMINANCE;
            break;
        case STBI_grey_alpha:
            image->_format = LUMINANCE_ALPHA;
            break;
        case STBI_rgb:
            image->_format = RGB;
            break;
        case STBI_rgb_alpha:
            image->_format = RGBA;
            break;
        default://STBI_default
            break;
    }
    image->_buffer = idata;
    image->_create_mode = 1;
    return image;
}
Image* Image::createEmpty(Format format, int width, int height)
{
    Image* image = new /*(std::nothrow)*/ Image;
    image->_width = width;
    image->_height = height;
    image->_format = format;
    int pixel_bytes = 0;
    switch (format) {
        case RGBA:
            pixel_bytes = 4;
            break;
        case RGB:
            pixel_bytes = 3;
            break;
        case LUMINANCE:
            pixel_bytes = 1;
            break;
        case LUMINANCE_ALPHA:
            pixel_bytes = 2;
            break;
        default:
            delete image;
            return NULL;
            break;
    }
    
    image->_buffer = malloc(width * height * pixel_bytes);
    return image;
}
GRAPHICCORE_END
