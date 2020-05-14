//
//  Texture2D.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "Texture2D.h"
#include <new>

GRAPHICCORE_BEGIN

Texture2D* GeneralTexture2D::createFromImage(const Image* image)
{
    GeneralTexture2D* texture = new /*(std::nothrow)*/ GeneralTexture2D;
    texture->_width = image->width();
    texture->_height = image->height();
    GLuint gl_format = GL_RGBA;
    texture->_format = (Format)image->getFormat();
    switch (image->getFormat()) {
        case Image::LUMINANCE:
            gl_format = GL_LUMINANCE;
            break;
        case Image::LUMINANCE_ALPHA:
            gl_format = GL_LUMINANCE_ALPHA;
            break;
        case Image::RGB:
            gl_format = GL_RGB;
            break;
        case Image::RGBA:
            gl_format = GL_RGBA;
            break;
        default://STBI_default
            break;
    }
    
    glGenTextures(1, &texture->_textureid);
    glBindTexture(GL_TEXTURE_2D, texture->_textureid);
    glTexImage2D(GL_TEXTURE_2D, 0 ,gl_format, image->width() , image->height() , 0, gl_format, GL_UNSIGNED_BYTE, image->getBuffer());
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    return texture;
}

Texture2D* GeneralTexture2D::createTexture(Format format, int width, int height)
{
    GLuint gl_format = GL_RGBA;
    switch (format){
        case LUMINANCE:
            gl_format = GL_LUMINANCE;
            break;
        case LUMINANCE_ALPHA:
            gl_format = GL_LUMINANCE_ALPHA;
            break;
        case RGB:
            gl_format = GL_RGB;
            break;
        case RGBA:
            gl_format = GL_RGBA;
            break;
        case DEPTH:
            gl_format = GL_DEPTH_COMPONENT;
            break;
        default:
            break;
    }

    GeneralTexture2D *texture = new /*(std::nothrow)*/ GeneralTexture2D;
    texture->_width = width;
    texture->_height = height;
    texture->_format = format;
    texture->_gl_format = gl_format;

    glGenTextures(1, &texture->_textureid);
    glBindTexture(GL_TEXTURE_2D, texture->_textureid);
    if (gl_format == GL_DEPTH_COMPONENT) {
        glTexImage2D(GL_TEXTURE_2D, 0 ,gl_format, width , height , 0, gl_format, GL_UNSIGNED_INT, NULL);
    }else
        glTexImage2D(GL_TEXTURE_2D, 0 ,gl_format, width , height , 0, gl_format, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return texture;
}

bool GeneralTexture2D::updateFromImage(const Image* image)
{
    if ((Format)image->getFormat() != _format) {
        return false;
    }
    
    glBindTexture(GL_TEXTURE_2D, _textureid);
    glTexImage2D(GL_TEXTURE_2D, 0 ,_gl_format, image->width() , image->height() , 0, _gl_format, GL_UNSIGNED_BYTE, image->getBuffer());
    return true;
}

void GeneralTexture2D::release()
{
   if (_textureid){
       glDeleteTextures(1, &_textureid);
       _textureid = 0;
   }
   _width = 0;
   _height = 0;
}


void DuplicateTexture2D::updateTexture(GLuint textureid, Format format, int width, int height) {
    _width = width;
    _height = height;
    _format = format;
    _textureid = textureid;
}

GRAPHICCORE_END
