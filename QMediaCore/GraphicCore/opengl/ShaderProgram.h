//
//  ShaderProgram.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_SHADERPROGRAM_H
#define GRAPHICCORE_SHADERPROGRAM_H

#include "GLMacros.h"
#include <map>
#include <vector>
#include <string>

GRAPHICCORE_BEGIN

class GlBinder{
public:
    GlBinder():_location(0),_buffer(0){}
    GLint _location;
    GLuint _buffer;
};

template <typename type>
class BufferArray{
public:
    BufferArray(){}
    ~BufferArray(){}

    void copyForm(const type * buffer, int count){
        _buffer.resize(count);
        memcpy(_buffer.data(), buffer, count * sizeof(type));
    }

    std::vector<type> _buffer;
};

class VertexAttrib {
public:
    VertexAttrib(){}
    ~VertexAttrib(){}

    enum Type{
        NONE = 0,
        VERTEX2 = 1,
        VERTEX3 = 1 << 1,
        VERTEX4 = 1 << 2,
        TEXCOORD = VERTEX2,
        NORMAL = 1 << 3
    };

    using Value = BufferArray<float> ;

    Type _type;
    GlBinder _glbinder;
    Value _value;
};

using IndicesArray = BufferArray<unsigned int>;

class Uniform{
public:
    Uniform(){}
    ~Uniform(){}

    enum Type{
        NONE = 0,
        INT,
        INTV,
        INT2,
        INT2V,
        INT3,
        INT3V,
        INT4,
        INT4V,
        FLOAT,
        FLOATV,
        FLOAT2,
        FLOAT2V,
        FLOAT3,
        FLOAT3V,
        FLOAT4,
        FLOAT4V,
        MATRIX3,
        MATRIX3V,
        MATRIX4,
        MATRIX4V,
        TEXTURE
    };

    class Value{
    public:
        void copyForm(const float * buffer, int count){
            _floatOrmatrix_array.resize(count);
            memcpy(_floatOrmatrix_array.data(), buffer, count * sizeof(float));
        }
        void copyForm(const int * buffer, int count){
            _int_array.resize(count);
            memcpy(_int_array.data(), buffer, count * sizeof(int));
        }

        unsigned int _texture;
        int _int_val;
        std::vector<int> _int_array;
        float _float_val;
        std::vector<float> _floatOrmatrix_array;
    };

    Type _type;
    GlBinder _glbinder;
    Value _value;
};

class ShaderProgram final{
public:
    
    enum DrawMode {
         RIANGLES = 0,
         RIANGLE_STRIP,
         TRIANGLE_FAN,
    };
    
    ShaderProgram();
    ~ShaderProgram();

    void addVertexAttribOption(const char* key, VertexAttrib::Type type);
    void addUniformOption(const char* key, Uniform::Type type);
    bool setVertexAttribValue(const char* key, VertexAttrib::Value &value);
    bool setVertexAttribValue(const char* key, int size, float floatArray[]);
    bool setUniformValue(const char* key, Uniform::Value &value);
    bool setUniformValue(const char* key, int iValue);
    bool setUniformValue(const char* key, float fValue);
    bool setUniformValue(const char* key, int size, int intArray[]);
    bool setUniformValue(const char* key, int size, float floatArray[]);
    void setIndicesValue(IndicesArray &value);

    int drawIndices(DrawMode mode, int count);
    int draw(DrawMode mode, int count);
    int drawRect();

    bool createProgram(const char* vshader, const char* fshader);
    void releaseProgram();
//    bool setProgramLocations();
    bool use();

    void enableBlend(bool bBlend){_enable_blend = bBlend;}
    void enableVBO(bool bVob){_enable_vbo = bVob;}
private:
    bool setUniformValue(Uniform& uniform, int size, int intArray[]);
    bool setUniformValue(Uniform& uniform, int size, float floatArray[]);
    static GLuint LoadShader(GLenum type, const char* shaderSrc);

    bool beginDraw();

    std::map<std::string, VertexAttrib > _attrib_maps;
    std::map<std::string, Uniform > _uniform_maps;

    bool _enable_vbo;
    bool _enable_blend;

    IndicesArray _elementIndices;

    GLuint _program;
    GLuint _vShader;
    GLuint _fShader;
};

GRAPHICCORE_END

#endif //APLAYERANDROID_STUDIO_SHADERPROGRAM_H
