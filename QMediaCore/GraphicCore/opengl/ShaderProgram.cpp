//
//  ShaderProgram.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "ShaderProgram.h"
#include "GLEngine.h"
#include "Utils/Logger.h"

GRAPHICCORE_BEGIN

inline GLuint getGlDrawMode(ShaderProgram::DrawMode mode){
    switch (mode){
        case ShaderProgram::RIANGLES:
            return GL_TRIANGLES;
        case ShaderProgram::RIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case ShaderProgram::TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
        default:
            return GL_TRIANGLE_STRIP;
    }
}

ShaderProgram::ShaderProgram():
_enable_vbo(true),
_blendFunc(BlendFunc::DISABLE),
_program(0),
_vShader(0),
_fShader(0) {

}
ShaderProgram::~ShaderProgram() {

}

void ShaderProgram::addVertexAttribOption(const char* key, VertexAttrib::Type type)
{
    VertexAttrib value;
    value._type = type;
    value._glbinder._location = glGetAttribLocation(_program, key);
    glCheckError();
//            if (_enable_vbo){
//                glGenBuffers(1,&binder._buffer);
//                glBindBuffer(GL_ARRAY_BUFFER,binder._buffer);
//                glBufferData(GL_ARRAY_BUFFER, item.second._value._buffer.size() * sizeof(float), item.second._value._buffer.data(),GL_STATIC_DRAW);
//                int vec_count = 3;
//                if(item.second._type == VertexAttrib::TEXCOORD)
//                    vec_count = 2;
//                glVertexAttribPointer(binder._location, vec_count , GL_FLOAT, GL_TRUE, vec_count * sizeof(float), 0);
//            }
    _attrib_maps.insert(std::make_pair(key,value));
}
void ShaderProgram::addUniformOption(const char* key, Uniform::Type type)
{
    Uniform value;
    value._type = type;
    value._glbinder._location = glGetUniformLocation(_program, key);
    glCheckError();
    _uniform_maps.insert(std::make_pair(key,value));
}

bool ShaderProgram::setVertexAttribValue(const char* key, VertexAttrib::Value &value)
{
    auto iter = _attrib_maps.find(key);
    if (iter != _attrib_maps.end()){
        iter->second._value = std::move(value);
        auto &attrib = iter->second;
//        attrib._glbinder._location = glGetAttribLocation(_program, item.first.c_str());
        int vec_count = 3;
        if(attrib._type == VertexAttrib::VERTEX3)
            vec_count = 3;
        else if(attrib._type == VertexAttrib::VERTEX4)
            vec_count = 4;
        else if(attrib._type == VertexAttrib::TEXCOORD)
            vec_count = 2;
        glVertexAttribPointer(attrib._glbinder._location, vec_count , GL_FLOAT, GL_TRUE, vec_count * sizeof(float), attrib._value._buffer.data());

        return true;
    }
    return false;
}

bool ShaderProgram::setVertexAttribValue(const char* key, int size, float floatArray[])
{
    auto iter = _attrib_maps.find(key);
    if (iter != _attrib_maps.end()){
        auto &attrib = iter->second;
//        attrib._glbinder._location = glGetAttribLocation(_program, item.first.c_str());
        int vec_count = 3;
        if(attrib._type == VertexAttrib::VERTEX3)
            vec_count = 3;
        else if(attrib._type == VertexAttrib::VERTEX4)
            vec_count = 4;
        else if(attrib._type == VertexAttrib::TEXCOORD)
            vec_count = 2;
        glVertexAttribPointer(attrib._glbinder._location, vec_count , GL_FLOAT, GL_TRUE, vec_count * sizeof(float), floatArray);

        return true;
    }
    return false;
}

bool ShaderProgram::setUniformValue(const char* key, Uniform::Value &value)
{
    auto iter = _uniform_maps.find(key);
    if (iter != _uniform_maps.end()){
        iter->second._value = std::move(value);
        auto &uniform = iter->second;
        auto &int_array = uniform._value._int_array;
        auto &float_array = uniform._value._floatOrmatrix_array;
        switch (uniform._type){
            case Uniform::INT:
                glUniform1i(uniform._glbinder._location, uniform._value._int_val);
                break;
            case Uniform::INTV:
                glUniform1iv(uniform._glbinder._location, int_array.size() ,int_array.data());
                break;
            case Uniform::INT2:
                glUniform2i(uniform._glbinder._location, int_array.data()[0] ,int_array.data()[1]);
                break;
            case Uniform::INT2V:
                glUniform2iv(uniform._glbinder._location, int_array.size() /2 ,int_array.data());
                break;
            case Uniform::INT3:
                glUniform3i(uniform._glbinder._location, int_array.data()[0] ,int_array.data()[1],int_array.data()[2]);
                break;
            case Uniform::INT3V:
                glUniform3iv(uniform._glbinder._location, int_array.size() /3 ,int_array.data());
                break;
            case Uniform::INT4:
                glUniform4i(uniform._glbinder._location, int_array.data()[0] ,int_array.data()[1],int_array.data()[2],int_array.data()[3]);
                break;
            case Uniform::INT4V:
                glUniform4iv(uniform._glbinder._location, int_array.size() /4 ,int_array.data());
                break;
            case Uniform::FLOAT:
                glUniform1f(uniform._glbinder._location, uniform._value._float_val);
                break;
            case Uniform::FLOATV:
                glUniform1fv(uniform._glbinder._location, float_array.size() ,float_array.data());
                break;
            case Uniform::FLOAT2:
                glUniform2f(uniform._glbinder._location, float_array.data()[0] ,float_array.data()[1]);
                break;
            case Uniform::FLOAT2V:
                glUniform2fv(uniform._glbinder._location, float_array.size() /2 ,float_array.data());
                break;
            case Uniform::FLOAT3:
                glUniform3f(uniform._glbinder._location, float_array.data()[0] ,float_array.data()[1],float_array.data()[2]);
                break;
            case Uniform::FLOAT3V:
                glUniform3fv(uniform._glbinder._location, float_array.size() /3 ,float_array.data());
                break;
            case Uniform::FLOAT4:
                glUniform4f(uniform._glbinder._location, float_array.data()[0] ,float_array.data()[1],float_array.data()[2],float_array.data()[3]);
                break;
            case Uniform::FLOAT4V:
                glUniform4fv(uniform._glbinder._location, float_array.size() /4 ,float_array.data());
                break;
            case Uniform::MATRIX3V:
                glUniformMatrix3fv(uniform._glbinder._location, float_array.size() / 9 , GL_FALSE ,float_array.data());
                break;
            case Uniform::MATRIX4V:
                glUniformMatrix4fv(uniform._glbinder._location, float_array.size() / 16 , GL_FALSE ,float_array.data());
                break;
        }

        return true;
    }
    return false;
}

bool ShaderProgram::setUniformValue(const char* key, int iValue){
    return setUniformValue(key, 1, (int*)&iValue);
}
bool ShaderProgram::setUniformValue(const char* key, float fValue){
    return setUniformValue(key, 1, (float*)&fValue);
}

bool ShaderProgram::setUniformValue(const char* key, int size, int intArray[]){
    auto iter = _uniform_maps.find(key);
    if (iter != _uniform_maps.end()){
        return setUniformValue(iter->second, size, intArray);
    }
    return false;
}
bool ShaderProgram::setUniformValue(const char* key, int size, float floatArray[]){
    auto iter = _uniform_maps.find(key);
    if (iter != _uniform_maps.end()){
        return setUniformValue(iter->second, size, floatArray);
    }
    return false;
}

bool ShaderProgram::setUniformValue(Uniform& uniform, int size, int intArray[]){
    GLint location = uniform._glbinder._location;
    Uniform::Type type = uniform._type;
    switch (type){
    case Uniform::TEXTURE:
        uniform._value._texture = intArray[0];
        return true;
    case Uniform::INT:
        uniform._value._int_val = intArray[0];
        glUniform1i(location, intArray[0]);
        return true;
    case Uniform::INTV:
        glUniform1iv(location, size, intArray);
        return true;
    case Uniform::INT2:
        glUniform2i(location, intArray[0] ,intArray[1]);
        return true;
    case Uniform::INT2V:
        glUniform2iv(location, size /2 ,intArray);
        return true;
    case Uniform::INT3:
        glUniform3i(location, intArray[0] ,intArray[1],intArray[2]);
        return true;
    case Uniform::INT3V:
        glUniform3iv(location, size /3 ,intArray);
        return true;
    case Uniform::INT4:
        glUniform4i(location, intArray[0] ,intArray[1],intArray[2],intArray[3]);
        return true;
    case Uniform::INT4V:
        glUniform4iv(location, size /4 ,intArray);
        return true;
    }
    std::copy(intArray, intArray + size, uniform._value._int_array.begin());
    return false;
}
bool ShaderProgram::setUniformValue(Uniform& uniform, int size, float floatArray[]){
    GLint location = uniform._glbinder._location;
    Uniform::Type type = uniform._type;
    switch (type){
    case Uniform::FLOAT:
        uniform._value._float_val = floatArray[0];
        glUniform1f(location, floatArray[0]);
        return true;
    case Uniform::FLOATV:
        glUniform1fv(location, size, floatArray);
        return true;
    case Uniform::FLOAT2:
        glUniform2f(location, floatArray[0] ,floatArray[1]);
        return true;
    case Uniform::FLOAT2V:
        glUniform2fv(location, size /2 ,floatArray);
        return true;
    case Uniform::FLOAT3:
        glUniform3i(location, floatArray[0] ,floatArray[1],floatArray[2]);
        return true;
    case Uniform::FLOAT3V:
        glUniform3fv(location, size /3 ,floatArray);
        return true;
    case Uniform::FLOAT4:
        glUniform4f(location, floatArray[0] ,floatArray[1],floatArray[2],floatArray[3]);
        return true;
    case Uniform::FLOAT4V:
        glUniform4fv(location, size /4 ,floatArray);
        return true;
    case Uniform::MATRIX3:
        glUniformMatrix3fv(location, 1, GL_FALSE, floatArray);
        return true;
    case Uniform::MATRIX3V:
        glUniformMatrix3fv(location, size/9 , GL_FALSE, floatArray);
        return true;
    case Uniform::MATRIX4:
        glUniformMatrix4fv(location, 1 , GL_FALSE, floatArray);
        return true;
    case Uniform::MATRIX4V:
        glUniformMatrix3fv(location, size/16 , GL_FALSE, floatArray);
        return true;
    }
    std::copy(floatArray, floatArray + size, uniform._value._floatOrmatrix_array.begin());
    return false;
}

void ShaderProgram::setIndicesValue(IndicesArray &value)
{
    _elementIndices = std::move(value);
}

bool ShaderProgram::createProgram(const char* vertex_shader, const char* fragment_shader)
{
    _vShader = LoadShader(GL_VERTEX_SHADER,vertex_shader);
    if (_vShader == 0) {
//        logerror("LoadShader GL_VERTEX_SHADER faild");
        glCheckError();
        return false;
    }
    _fShader = LoadShader(GL_FRAGMENT_SHADER,fragment_shader);
    if (_fShader == 0) {
//        logerror("LoadShader GL_FRAGMENT_SHADER faild");
        return false;
    }

    _program = glCreateProgram();
    if (!_program)
    {
//        logerror("glCreateProgram failed");
        return false;
    }

    glAttachShader(_program, _vShader);
    glAttachShader(_program, _fShader);

    GLint linked;
    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &linked);

    if (!linked)
    {
//        logerror("glLinkProgram failed");
        releaseProgram();
        return false;
    }

    return true;
}
void ShaderProgram::releaseProgram()
{
    for(auto& item : _attrib_maps){
        auto& binder = item.second._glbinder;
        if (binder._buffer){
            glDeleteBuffers(1,&binder._buffer);
            binder._buffer = 0;
        }
        binder._location = 0;
    }
    for(auto& item : _uniform_maps){
        auto& binder = item.second._glbinder;
        if (binder._buffer){
            glDeleteBuffers(1,&binder._buffer);
            binder._buffer = 0;
        }
        binder._location = 0;
    }
    glUseProgram(0);
    if (_program){
        glDeleteProgram(_program);
        _program = 0;
    }
    if (_vShader){
        glDeleteShader(_vShader);
        _vShader = 0;
    }
    if (_fShader){
        glDeleteShader(_fShader);
        _fShader = 0;
    }
}

bool ShaderProgram::use() {
    if (_program == 0)
        return false;

    GLint currProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM,&currProgram);
    if (currProgram != _program) {
        glUseProgram(_program);
        if(glGetError() != GL_NO_ERROR) {
            LOGW("glUseProgram %d faild",_program);
            return false;
        }
    }
    return true;
}

bool ShaderProgram::beginDraw()
{
    if(use()){
        for (auto &item : _attrib_maps) {
            glEnableVertexAttribArray(item.second._glbinder._location);
        }

        int textureId = 0;
        for (auto &item : _uniform_maps) {
            auto &uniform = item.second;
            if (uniform._type == Uniform::TEXTURE){
                glActiveTexture(GL_TEXTURE0 + textureId);
                glBindTexture(uniform._value._textureTarget, uniform._value._texture);
                glUniform1i(uniform._glbinder._location, textureId);

                textureId++;
            }
        }
        if (_blendFunc != BlendFunc::DISABLE) {
            glEnable(GL_BLEND);
//            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendFunc(_blendFunc.src, _blendFunc.dst);
        }else
            glDisable(GL_BLEND);
    
        return true;
    }
    LOGE("ShaderProgram::beginDraw failed..");
    return false;
}

GLuint ShaderProgram::LoadShader(GLenum type, const char* shaderSrc) {

    GLuint shader = glCreateShader(type);

    if (!shader)
    {
        glCheckError();
        return 0;
    }

    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCheckError();

    glCompileShader(shader);
    glCheckError();
    glReleaseShaderCompiler();

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled)
    {
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        if ( info_len > 1 )
        {
           char *info_log = (char*)malloc ( sizeof ( char ) * info_len );
           glGetShaderInfoLog ( shader, info_len, NULL, info_log );

           free ( info_log );
           info_log = NULL;
        }
        glCheckError();
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int ShaderProgram::drawIndices(DrawMode mode, int count)
{
    if (beginDraw()){
        glDrawElements(getGlDrawMode(mode),count,GL_UNSIGNED_INT,_elementIndices._buffer.data());

        return 0;
    }

    return -1;
}
int ShaderProgram::draw(DrawMode mode, int count)
{
    if (beginDraw()){
        glDrawArrays(getGlDrawMode(mode),0, count);

        return 0;
    }

    return -1;
}
int ShaderProgram::drawRect()
{
    if (beginDraw()){
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        return 0;
    }

    return -1;
}

GRAPHICCORE_END
