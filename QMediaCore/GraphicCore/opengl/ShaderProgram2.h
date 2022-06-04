//
//  ShaderProgram2.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_SHADERPROGRAM2_H
#define GRAPHICCORE_SHADERPROGRAM2_H

#include "GLMacros.h"
#include "GraphicCore/base/ColorTypes.h"
#include <unordered_map>
#include <vector>
#include <string>
#include "Utils/Variant.h"

GRAPHICCORE_BEGIN

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
    
    using Value = Variant ;
    
    VertexAttrib() = default;
    VertexAttrib(Type type, Value& _value): _type(type),  _value(_value){}
    ~VertexAttrib() = default;

    Type _type;
    Value _value;
};

class Uniform {
public:
    
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
        TEXTURE,
        MAX_UNIFORM_TYPE
    };
    
    struct TextureUnit {
        unsigned int texture_id;
        GLenum texture_target;
    };
    
    template <typename T>
    struct alignas(sizeof(T)) Vec2{
        Vec2():x(0),y(0) {}
        Vec2(T xx, T yy):x(xx),y(yy) {}
        T x, y;
    };
    
    template <typename T>
    struct alignas(sizeof(T)) Vec3 {
        Vec3():x(0),y(0),z(0) {}
        Vec3(T xx, T yy, T zz):x(xx),y(yy),z(zz) {}
        Vec3(Vec2<T> v2, T zz = 0):x(v2.x),y(v2.y),z(zz) {}
        T x, y, z;
    };
    
    template <typename T>
    struct alignas(sizeof(T)) Vec4 {
        Vec4():x(0),y(0),z(0),w(0) {}
        Vec4(T xx, T yy, T zz, T ww):x(xx),y(yy),z(zz),w(ww) {}
        Vec4(Vec3<T> v3, T ww = 0):x(v3.x),y(v3.y),z(v3.z),w(ww) {}
        T x, y, z, w;
    };
    
    struct Matrix {
        Matrix(): level(0), count(0) {}
        Matrix(float data[], int l, int c = 1): mdata(data, data + (l*l*c)), level(l), count(c) {}
        Matrix(std::vector<float> data, int l, int c = 1): mdata(std::move(data)), level(l), count(c) {}
        int level, count;
        std::vector<float> mdata;
    };
    
    using Value = Variant;
    
    Uniform(){}
    Uniform(Type type, Value& _value): _type(type), _value(_value){}
    ~Uniform(){}

    Type _type;
    Value _value;
};

class ShaderProgram2 final{
public:
    
    enum DrawMode {
         RIANGLES = 0,
         RIANGLE_STRIP,
         TRIANGLE_FAN,
    };
    
    ShaderProgram2();
    ~ShaderProgram2();
    
    // setVertexAttribValue immediately
    void setVertexAttribValue(const char* key, Uniform::Vec2<float> value[], int count);
    void setVertexAttribValue(const char* key, Uniform::Vec3<float> value[], int count);
    void setVertexAttribValue(const char* key, Uniform::Vec4<float> value[], int count);
    
    // setUniformValue immediately
    void setUniformValue(const char* key, int value);
    void setUniformValue(const char* key, int value[], int count);
    void setUniformValue(const char* key, float value);
    void setUniformValue(const char* key, float value[], int count);
    void setUniformValue(const char* key, Uniform::Vec2<int> value);
    void setUniformValue(const char* key, Uniform::Vec2<int> value[], int count);
    void setUniformValue(const char* key, Uniform::Vec2<float> value);
    void setUniformValue(const char* key, Uniform::Vec2<float> value[], int count);
    void setUniformValue(const char* key, Uniform::Vec3<int> value);
    void setUniformValue(const char* key, Uniform::Vec3<int> value[], int count);
    void setUniformValue(const char* key, Uniform::Vec3<float> value);
    void setUniformValue(const char* key, Uniform::Vec3<float> value[], int count);
    void setUniformValue(const char* key, Uniform::Vec4<int> value);
    void setUniformValue(const char* key, Uniform::Vec4<int> value[], int count);
    void setUniformValue(const char* key, Uniform::Vec4<float> value);
    void setUniformValue(const char* key, Uniform::Vec4<float> value[], int count);
    
    void setUniformValue(const char* key, Uniform::Matrix matrix);
    void setUniformValue(const char* key, Uniform::TextureUnit texutre_unit);
    
    //set draw index array
    void setIndicesValue(Variant &value);
    void setIndicesValue(unsigned int value[], int size);

    int drawIndices(DrawMode mode, int count);
    int draw(DrawMode mode, int count);
    int drawRectangle();

    bool createProgram(const char* vertex_shader, const char* fragment_shader);
    void releaseProgram();
    
    bool use();

    void setBlendFunc(BlendFunc blend_func) { blend_func_ = blend_func; }
    void enableVBO(bool bVbo){enable_vbo_ = bVbo;}
private:
    
    struct GlBinder{
        GlBinder():location(-1),buffer(0){}
        GlBinder(GLint locat, GLuint buff):location(locat),buffer(buff) {}
        GLint location;
        GLuint buffer;
    };
    
    typedef struct AttribStorage {
        unsigned int vec_fmt;
        int vec_size;
        GlBinder gl_binder;
        Variant value;
    } AttribStorage;
    
    typedef struct TextureStorage {
        Uniform::TextureUnit texture_unit;
        GlBinder gl_binder;
    } TextureStorage;

    
    static GLuint LoadShader(GLenum type, const char* shaderSrc);
    GLint GetUniformLocation(const char* key);

    void setVertexAttribValue(const char* key, float data[], int data_size, int vec_size);
    void prepareTextures(const TextureStorage& texture_storage, unsigned int& texture_idx);
    void prepareAttribs(const AttribStorage& attrib);
    bool beginDraw();

    std::unordered_map<std::string, AttribStorage > attrib_storage_;
    std::unordered_map<std::string, TextureStorage > texture_storage_;

    Variant element_storage_;

    GLuint program_;
    GLuint vert_shader_, frag_shader_;
    
    bool enable_vbo_;
    BlendFunc blend_func_;
};

GRAPHICCORE_END

#endif //APLAYERANDROID_STUDIO_SHADERPROGRAM_H
