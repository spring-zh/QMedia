//
//  ShaderProgram2.cpp
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
_enable_vbo(false),
_blendFunc(BlendFunc::DISABLE),
_program(0),
_vShader(0), _fShader(0) {

}
ShaderProgram::~ShaderProgram() {

}

void ShaderProgram::setVertexAttribValue(const char* key, VertexAttrib::Type type, VertexAttrib::Value &value) {
    auto iter = _attrib_maps.find(key);
    if (iter == _attrib_maps.end()) { //first set
        VertexAttrib attrib;
        attrib._type = type;
        attrib._value = value;
        attrib._glbinder._location = glGetAttribLocation(_program, key);
        if (_enable_vbo){
            glGenBuffers(1,&attrib._glbinder._buffer);
            glBindBuffer(GL_ARRAY_BUFFER, attrib._glbinder._buffer);
            glBufferData(GL_ARRAY_BUFFER, attrib._value._buffer.size() * sizeof(float), attrib._value._buffer.data(), GL_DYNAMIC_DRAW);
        }
        _attrib_maps.insert(std::make_pair(key, std::move(attrib)));
    }else { //update attrib value
        VertexAttrib& attrib = iter->second;
        attrib._value = value;
        attrib._type = type;
        if (_enable_vbo && attrib._glbinder._buffer) {
            glBindBuffer(GL_ARRAY_BUFFER, attrib._glbinder._buffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, attrib._value._buffer.size() * sizeof(float), attrib._value._buffer.data());
        }
    }
}

void ShaderProgram::setVertexAttribValue(const char* key, VertexAttrib::Type type, float value[], int size) {
    VertexAttrib::Value attrib;
    attrib.copyForm(value, size);
    setVertexAttribValue(key, type, attrib);
}

void ShaderProgram::setUniformValue(const char* key, Uniform::Type type, Uniform::Value &value) {
    auto iter = _uniform_maps.find(key);
    if (iter == _uniform_maps.end()) { //first set
        Uniform uniform;
        uniform._type = type;
        uniform._value = value;
        uniform._glbinder._location = glGetUniformLocation(_program, key);
        
        _uniform_maps.insert(std::make_pair(key, std::move(uniform)));
    }else {  //update uniform value
        Uniform& uniform = iter->second;
        uniform._value = value;
        uniform._type = type;
    }
}

void ShaderProgram::setUniformValue(const char* key, Uniform::Type type, int value) {
    Uniform::Value uniform_val;
    if (type == Uniform::TEXTURE) {
        uniform_val._texture = value;
    }
    uniform_val._int_val = value;
    setUniformValue(key, type, uniform_val);
}
void ShaderProgram::setUniformValue(const char* key, Uniform::Type type, float value) {
    Uniform::Value uniform_val;
    uniform_val._float_val = value;
    setUniformValue(key, type, uniform_val);
}
void ShaderProgram::setUniformValue(const char* key, Uniform::Type type, int value[], int size) {
    Uniform::Value uniform_val;
    uniform_val.copyForm(value, size);
    setUniformValue(key, type, uniform_val);
}
void ShaderProgram::setUniformValue(const char* key, Uniform::Type type, float value[], int size) {
    Uniform::Value uniform_val;
    uniform_val.copyForm(value, size);
    setUniformValue(key, type, uniform_val);
}

void ShaderProgram::updateUniformValue(const char* key, Uniform::Value &value) {
    auto iter = _uniform_maps.find(key);
    if (iter != _uniform_maps.end()) { //first set
        Uniform& uniform = iter->second;
        uniform._value = value;
    }
}
    
bool ShaderProgram::prepareUniform(Uniform& uniform, unsigned int& texture_idx) {
    auto& int_array = uniform._value._int_array;
    GLsizei int_size = static_cast<GLsizei>(int_array.size());
    auto& float_array = uniform._value._floatOrmatrix_array;
    GLsizei float_size = static_cast<GLsizei>(float_array.size());
    switch (uniform._type){
    case Uniform::INT:
        glUniform1i(uniform._glbinder._location, uniform._value._int_val);
        break;
    case Uniform::INTV:
        glUniform1iv(uniform._glbinder._location, int_size ,int_array.data());
        break;
    case Uniform::INT2:
        glUniform2i(uniform._glbinder._location, int_array.data()[0] ,int_array.data()[1]);
        break;
    case Uniform::INT2V:
        glUniform2iv(uniform._glbinder._location, int_size /2 ,int_array.data());
        break;
    case Uniform::INT3:
        glUniform3i(uniform._glbinder._location, int_array.data()[0] ,int_array.data()[1],int_array.data()[2]);
        break;
    case Uniform::INT3V:
        glUniform3iv(uniform._glbinder._location, int_size /3 ,int_array.data());
        break;
    case Uniform::INT4:
        glUniform4i(uniform._glbinder._location, int_array.data()[0] ,int_array.data()[1],int_array.data()[2],int_array.data()[3]);
        break;
    case Uniform::INT4V:
        glUniform4iv(uniform._glbinder._location, int_size /4 ,int_array.data());
        break;
    case Uniform::FLOAT:
        glUniform1f(uniform._glbinder._location, uniform._value._float_val);
        break;
    case Uniform::FLOATV:
        glUniform1fv(uniform._glbinder._location, float_size ,float_array.data());
        break;
    case Uniform::FLOAT2:
        glUniform2f(uniform._glbinder._location, float_array.data()[0] ,float_array.data()[1]);
        break;
    case Uniform::FLOAT2V:
        glUniform2fv(uniform._glbinder._location, float_size /2 ,float_array.data());
        break;
    case Uniform::FLOAT3:
        glUniform3f(uniform._glbinder._location, float_array.data()[0] ,float_array.data()[1],float_array.data()[2]);
        break;
    case Uniform::FLOAT3V:
        glUniform3fv(uniform._glbinder._location, float_size /3 ,float_array.data());
        break;
    case Uniform::FLOAT4:
        glUniform4f(uniform._glbinder._location, float_array.data()[0] ,float_array.data()[1],float_array.data()[2],float_array.data()[3]);
        break;
    case Uniform::FLOAT4V:
        glUniform4fv(uniform._glbinder._location, float_size /4 ,float_array.data());
        break;
    case Uniform::MATRIX3:
    case Uniform::MATRIX3V:
        glUniformMatrix3fv(uniform._glbinder._location, float_size / 9 , GL_FALSE ,float_array.data());
        break;
    case Uniform::MATRIX4:
    case Uniform::MATRIX4V:
        glUniformMatrix4fv(uniform._glbinder._location, float_size / 16 , GL_FALSE ,float_array.data());
        break;
    case Uniform::TEXTURE:
        {
            glActiveTexture(GL_TEXTURE0 + texture_idx);
            glBindTexture(uniform._value._textureTarget, uniform._value._texture);
            glUniform1i(uniform._glbinder._location, texture_idx);
            texture_idx++;
        }
        break;
    default:
        return false;
    }
    return true;
}
bool ShaderProgram::prepareVertexAttrib(VertexAttrib& attrib) {
    int vec_count = 3;
    if(attrib._type == VertexAttrib::VERTEX3)
        vec_count = 3;
    else if(attrib._type == VertexAttrib::VERTEX4)
        vec_count = 4;
    else if(attrib._type == VertexAttrib::TEXCOORD)
        vec_count = 2;
    glEnableVertexAttribArray(attrib._glbinder._location);
    if (_enable_vbo && (attrib._glbinder._buffer > 0)) {
        glBindBuffer(GL_ARRAY_BUFFER, attrib._glbinder._buffer);
//        glBufferData(GL_ARRAY_BUFFER, attrib._value._buffer.size() * sizeof(float), attrib._value._buffer.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(attrib._glbinder._location, vec_count , GL_FLOAT, GL_TRUE, vec_count * sizeof(float), (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }else {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribPointer(attrib._glbinder._location, vec_count , GL_FLOAT, GL_TRUE, vec_count * sizeof(float), attrib._value._buffer.data());
    }
    return true;
}

void ShaderProgram::setIndicesValue(IndicesArray &value)
{
    _elementIndices = std::move(value);
}
void ShaderProgram::setIndicesValue(unsigned int value[], int size) {
    IndicesArray indices_array;
    indices_array.copyForm(value, size);
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
        if (_enable_vbo && binder._buffer){
            glDeleteBuffers(1,&binder._buffer);
            binder._buffer = 0;
        }
        binder._location = -1;
    }
    for(auto& item : _uniform_maps){
        auto& binder = item.second._glbinder;
        if (_enable_vbo && binder._buffer){
            glDeleteBuffers(1,&binder._buffer);
            binder._buffer = 0;
        }
        binder._location = -1;
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
        //set blend mode and BlendFunc
        if (_blendFunc != BlendFunc::DISABLE) {
            glEnable(GL_BLEND);
            glBlendFunc(_blendFunc.src, _blendFunc.dst);
        }else
            glDisable(GL_BLEND);
        
        //set vertexattrib values
        for (auto &item : _attrib_maps) {
            prepareVertexAttrib(item.second);
        }
        
        //set uniform values
        unsigned int texture_count = 0;
        for (auto &item : _uniform_maps) {
            prepareUniform(item.second, texture_count);
        }
        return true;
    }else
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
int ShaderProgram::drawRectangle()
{
    if (beginDraw()){
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        return 0;
    }

    return -1;
}

GRAPHICCORE_END
