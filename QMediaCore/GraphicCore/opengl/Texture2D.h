//
//  Texture2D.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_TEXTURE2D_H
#define GRAPHICCORE_TEXTURE2D_H

#include <stdint.h>
#include "GLMacros.h"
#include "../base/Image.h"

GRAPHICCORE_BEGIN

class Texture2D {
public:
//    enum Format : int{
//        RGBA = 0,
//        RGB,
//        LUMINANCE,
//        LUMINANCE_ALPHA,
//        UNKNOW
//    };
    using Format = Image::Format;
    
    virtual ~Texture2D() {}
    
    virtual GLuint getTextureId() const { return _textureid; }
    virtual Format getFormat() const { return _format; }
    virtual int width() const { return _width;}
    virtual int height() const { return _height;}

    virtual void release() = 0;
    
protected:
    Texture2D():_textureid(0), _width(0),_height(0),_format(Image::RGBA),_gl_format(GL_RGBA) {}
    Texture2D(GLuint textureid, Format format, int width, int height):
    _textureid(textureid),_format(format),_width(width),_height(height) {}
    GLuint _textureid;
    GLuint _gl_format;
    Format _format;
    int _width;
    int _height;
};

class GeneralTexture2D : public Texture2D{
public:
    ~GeneralTexture2D() {}
    
    static Texture2D* createFromImage(const Image* image);
    static Texture2D* createTexture(Format format, int width, int height);
    
    bool updateFromImage(const Image* image);

    void release() override;
protected:
    GeneralTexture2D() {}
};

class DuplicateTexture2D : public Texture2D {
public:
    DuplicateTexture2D():_textureid(0),_format(Image::UNKNOW),_width(0),_height(0) {}
    DuplicateTexture2D(GLuint textureid, Format format, int width, int height):
    Texture2D(textureid, format, width, height) {}
    
    ~DuplicateTexture2D() {}

    virtual void release() override {}
    
    void updateTexture(GLuint textureid, Format format, int width, int height);
private:
    GLuint _textureid;
    Format _format;
    int _width;
    int _height;
};

GRAPHICCORE_END

#endif //GRAPHICCORE_TEXTURE2D_H
