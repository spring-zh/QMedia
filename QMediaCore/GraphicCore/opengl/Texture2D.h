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
#include "GraphicCore/base/Image.h"

GRAPHICCORE_BEGIN

class Texture2D {
public:
    enum Format : int{
        UNKNOW = Image::UNKNOW,
        RGBA = Image::RGBA,
        RGB = Image::RGB,
        LUMINANCE = Image::LUMINANCE,
        LUMINANCE_ALPHA = Image::LUMINANCE_ALPHA,
        DEPTH = 100,
    };

    virtual ~Texture2D() {}
    
    virtual GLuint getTextureId() const { return _textureid; }
    virtual GLenum getTextureTarget() const { return _textureTarget; }
    virtual Format getFormat() const { return _format; }
    virtual int width() const { return _width;}
    virtual int height() const { return _height;}

    virtual void release() = 0;
    
protected:
    Texture2D():_textureid(0),  _textureTarget(GL_TEXTURE_2D), _width(0),_height(0),_format(RGBA),_gl_format(GL_RGBA) {}
    Texture2D(GLuint textureid, GLenum textureTarget, Format format, int width, int height):
    _textureid(textureid),_textureTarget(textureTarget), _format(format),_width(width),_height(height) {}
    GLuint _textureid;
    GLenum _textureTarget;
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
    DuplicateTexture2D():Texture2D(0,GL_TEXTURE_2D,UNKNOW,0,0) {}
    DuplicateTexture2D(GLuint textureid, GLenum textureTarget, Format format, int width, int height):
    Texture2D(textureid, textureTarget, format, width, height) {}
    
    ~DuplicateTexture2D() = default;

    virtual void release() override {}
    
    void updateTexture(GLuint textureid, Format format, int width, int height);
};

GRAPHICCORE_END

#endif //GRAPHICCORE_TEXTURE2D_H
