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

Image* Image::createFromFile(const char *filename)
{
    Image* image = nullptr;
    int w,h,n;
    stbi_uc *idata = stbi_load(filename, &w, &h, &n, 4);
    n = 4;
    if (idata != NULL) {
        image = new /*(std::nothrow)*/ Image;
        image->init(idata, w, h, n);
    }
    return image;
}

Image* Image::createFromBuffer(const uint8_t* buffer, int buffer_size)
{
    Image* image = nullptr;
    int w,h,n;
    stbi_uc *idata = stbi_load_from_memory(buffer, buffer_size, &w, &h, &n, 4);
    n = 4;
    if (idata != NULL) {
        image = new /*(std::nothrow)*/ Image;
        image->init(idata, w, h, n);
    }
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
            return nullptr;
    }
    
    image->_buffer = malloc(width * height * pixel_bytes);
    return image;
}

bool Image::init(uint8_t* buffer, int width, int height, int count) {
    _width = width;
    _height = height;
    switch (count) {
        case STBI_grey:
            _format = LUMINANCE;
            break;
        case STBI_grey_alpha:
            _format = LUMINANCE_ALPHA;
            break;
        case STBI_rgb:
            _format = RGB;
            break;
        case STBI_rgb_alpha:
            _format = RGBA;
            break;
        default://STBI_default
            break;
    }
    _buffer = buffer;
    _create_mode = 1;
    return true;
}

GRAPHICCORE_END
