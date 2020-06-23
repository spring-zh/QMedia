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
#include "GraphicCore/base/ColorTypes.h"
#include <map>
#include <vector>
#include <string>

GRAPHICCORE_BEGIN

class GlBinder{
public:
    GlBinder():_location(-1),_buffer(0){}
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
    enum Type{
        NONE = 0,
        VERTEX2 = 1,
        VERTEX3 = 1 << 1,
        VERTEX4 = 1 << 2,
        TEXCOORD = VERTEX2,
        NORMAL = 1 << 3
    };
    using Value = BufferArray<float> ;
    
    VertexAttrib() = default;
    ~VertexAttrib() = default;

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
        Value():_texture(0),_textureTarget(GL_TEXTURE_2D),_int_val(0),_float_val(0) {
        }
        void copyForm(const float * buffer, int count){
            _floatOrmatrix_array.resize(count);
            memcpy(_floatOrmatrix_array.data(), buffer, count * sizeof(float));
        }
        void copyForm(const int * buffer, int count){
            _int_array.resize(count);
            memcpy(_int_array.data(), buffer, count * sizeof(int));
        }

        unsigned int _texture;
        GLenum _textureTarget;
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

    // setVertexAttribValue
    void setVertexAttribValue(const char* key, VertexAttrib::Type type, VertexAttrib::Value &value);
    void setVertexAttribValue(const char* key, VertexAttrib::Type type, float value[], int size);
    
    // setUniformValue
    void setUniformValue(const char* key, Uniform::Type type, Uniform::Value &value);
    void setUniformValue(const char* key, Uniform::Type type, int value);
    void setUniformValue(const char* key, Uniform::Type type, float value);
    void setUniformValue(const char* key, Uniform::Type type, int value[], int size);
    void setUniformValue(const char* key, Uniform::Type type, float value[], int size);
    
    //update existing uniform
    void updateUniformValue(const char* key, Uniform::Value &value);
    
    //set draw index array
    void setIndicesValue(IndicesArray &value);
    void setIndicesValue(unsigned int value[], int size);

    int drawIndices(DrawMode mode, int count);
    int draw(DrawMode mode, int count);
    int drawRectangle();

    bool createProgram(const char* vertex_shader, const char* fragment_shader);
    void releaseProgram();
    
    bool use();

    void setBlendFunc(BlendFunc blendFunc) { _blendFunc = blendFunc; }
    void enableVBO(bool bVbo){_enable_vbo = bVbo;}
private:
    
    static GLuint LoadShader(GLenum type, const char* shaderSrc);

    bool prepareUniform(Uniform& uniform, unsigned int& texture_idx);
    bool prepareVertexAttrib(VertexAttrib& attrib);
    bool beginDraw();

    std::map<std::string, VertexAttrib > _attrib_maps;
    std::map<std::string, Uniform > _uniform_maps;

    IndicesArray _elementIndices;

    GLuint _program;
    GLuint _vShader, _fShader;
    
    bool _enable_vbo;
    BlendFunc _blendFunc;
};

GRAPHICCORE_END

#endif //APLAYERANDROID_STUDIO_SHADERPROGRAM_H
