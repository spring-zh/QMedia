//
//  ShaderProgram2.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include <stdlib.h>
#include "ShaderProgram2.h"
#include "GLEngine.h"
#include "Utils/Logger.h"

GRAPHICCORE_BEGIN

inline GLuint getGlDrawMode(ShaderProgram2::DrawMode mode){
    switch (mode){
        case ShaderProgram2::RIANGLES:
            return GL_TRIANGLES;
        case ShaderProgram2::RIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        case ShaderProgram2::TRIANGLE_FAN:
            return GL_TRIANGLE_FAN;
        default:
            return GL_TRIANGLE_STRIP;
    }
}

ShaderProgram2::ShaderProgram2():
enable_vbo_(false),
blend_func_(BlendFunc::DISABLE),
program_(0),
vert_shader_(0), frag_shader_(0) {

}
ShaderProgram2::~ShaderProgram2() {

}

void ShaderProgram2::setVertexAttribValue(const char* key, Uniform::Vec2<float> value[], int count) {
    setVertexAttribValue(key, (float*)value, count*2, 2);
}
void ShaderProgram2::setVertexAttribValue(const char* key, Uniform::Vec3<float> value[], int count) {
    setVertexAttribValue(key, (float*)value, count*3, 3);
}
void ShaderProgram2::setVertexAttribValue(const char* key, Uniform::Vec4<float> value[], int count) {
    setVertexAttribValue(key, (float*)value, count*4, 4);
}
void ShaderProgram2::setVertexAttribValue(const char* key, float data[], int data_size, int vec_size) {
    auto iter = attrib_storage_.find(key);
    if (iter == attrib_storage_.end()) { //first set
        AttribStorage attrib;
        attrib.vec_size = vec_size;
        attrib.vec_fmt = GL_FLOAT;
        attrib.value = Variant(data, data_size);
        attrib.gl_binder.location = GetUniformLocation(key);
        if (enable_vbo_){
            const auto& buffer = attrib.value.getVal<std::vector<float>>();
            glGenBuffers(1,&attrib.gl_binder.buffer);
            glBindBuffer(GL_ARRAY_BUFFER, attrib.gl_binder.buffer);
            glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(float), buffer.data(), GL_DYNAMIC_DRAW);
        }
        attrib_storage_.insert(std::make_pair(key, std::move(attrib)));
    }else {
        AttribStorage& attrib = iter->second;
        attrib.value = Variant(data, data_size);
        if (enable_vbo_ && attrib.gl_binder.buffer) {
            const auto& buffer = attrib.value.getVal<std::vector<float>>();
            glBindBuffer(GL_ARRAY_BUFFER, attrib.gl_binder.buffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.size() * sizeof(float), buffer.data());
        }
    }
}

void ShaderProgram2::setUniformValue(const char* key, int value) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform1i(location, value));
}
void ShaderProgram2::setUniformValue(const char* key, int value[], int count) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform1iv(location, count, value));
}
void ShaderProgram2::setUniformValue(const char* key, float value) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform1f(location, value));
}
void ShaderProgram2::setUniformValue(const char* key, float value[], int count) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform1fv(location, count, value));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec2<int> value) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform2i(location, value.x, value.y));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec2<int> value[], int count) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform2iv(location, count, (const int*)&value));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec2<float> value) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform2f(location, value.x, value.y));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec2<float> value[], int count) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform2fv(location, count, (const float*)&value));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec3<int> value) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform3i(location, value.x, value.y, value.z));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec3<int> value[], int count) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform3iv(location, count, (const int*)&value));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec3<float> value) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform3f(location, value.x, value.y, value.z));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec3<float> value[], int count) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform3fv(location, count, (const float*)&value));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec4<int> value) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform4i(location, value.x, value.y, value.z, value.w));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec4<int> value[], int count) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform4iv(location, count, (const int*)&value));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec4<float> value) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform4f(location, value.x, value.y, value.z, value.w));
}
void ShaderProgram2::setUniformValue(const char* key, Uniform::Vec4<float> value[], int count) {
    GLint location = GetUniformLocation(key);
    CHECK_GL_ERROR(glUniform4fv(location, count, (const float*)&value));
}

void ShaderProgram2::setUniformValue(const char* key, Uniform::Matrix matrix) {
    GLint location = GetUniformLocation(key);
    if (matrix.level == 2) {
        CHECK_GL_ERROR(glUniformMatrix2fv(location, matrix.count , GL_FALSE ,matrix.mdata.data()));
    }else if (matrix.level == 3) {
        CHECK_GL_ERROR(glUniformMatrix3fv(location, matrix.count , GL_FALSE ,matrix.mdata.data()));
    }else if (matrix.level == 4) {
        CHECK_GL_ERROR(glUniformMatrix4fv(location, matrix.count , GL_FALSE ,matrix.mdata.data()));
    }else
        assert(false);
}

void ShaderProgram2::setUniformValue(const char* key, Uniform::TextureUnit texutre_unit) {
    auto iter = texture_storage_.find(key);
    if (iter == texture_storage_.end()) { //first set
        TextureStorage texture;
        texture.gl_binder.location = GetUniformLocation(key);
        texture.texture_unit = texutre_unit;
        texture_storage_.insert(std::make_pair(key, std::move(texture)));
    }else {
        TextureStorage& texture = iter->second;
        texture.texture_unit = texutre_unit;
    }
}

void ShaderProgram2::prepareTextures(const TextureStorage& texture_storage, unsigned int& texture_idx) {
    const Uniform::TextureUnit& texture_unit = texture_storage.texture_unit;
    glActiveTexture(GL_TEXTURE0 + texture_idx);
    CHECK_GL_ERROR(glBindTexture(texture_unit.texture_target, texture_unit.texture_id));
    CHECK_GL_ERROR(glUniform1i(texture_storage.gl_binder.location, texture_idx));
    texture_idx++;
}

void ShaderProgram2::prepareAttribs(const AttribStorage& attrib) {
    const GlBinder& gl_binder = attrib.gl_binder;
    CHECK_GL_ERROR(glEnableVertexAttribArray(gl_binder.location));
    
    if (enable_vbo_ && (gl_binder.buffer > 0)) {
        CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, gl_binder.buffer));
//        glBufferData(GL_ARRAY_BUFFER, attrib._value._buffer.size() * sizeof(float), attrib._value._buffer.data(), GL_DYNAMIC_DRAW);
        CHECK_GL_ERROR(glVertexAttribPointer(gl_binder.location, attrib.vec_size , GL_FLOAT, GL_TRUE, attrib.vec_size * sizeof(float), (GLvoid*)0));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
    }else {
        const auto& buffer = attrib.value.getVal<std::vector<float>>();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(glVertexAttribPointer(gl_binder.location, attrib.vec_size , GL_FLOAT, GL_TRUE, attrib.vec_size * sizeof(float), buffer.data()));
    }
}

void ShaderProgram2::setIndicesValue(Variant &value)
{
    element_storage_ = std::move(value);
}
void ShaderProgram2::setIndicesValue(unsigned int value[], int size) {
    element_storage_ = Variant((int*)value, size);
}

bool ShaderProgram2::createProgram(const char* vertex_shader, const char* fragment_shader)
{
    vert_shader_ = LoadShader(GL_VERTEX_SHADER,vertex_shader);
    if (vert_shader_ == 0) {
        LOGE("LoadShader GL_VERTEX_SHADER faild: %s", vertex_shader);
        return false;
    }
    frag_shader_ = LoadShader(GL_FRAGMENT_SHADER,fragment_shader);
    if (frag_shader_ == 0) {
        LOGE("LoadShader GL_FRAGMENT_SHADER faild: %s", vertex_shader);
        return false;
    }

    CHECK_GL_ERROR(program_ = glCreateProgram());
    if (!program_)
    {
        return false;
    }

    CHECK_GL_ERROR(glAttachShader(program_, vert_shader_));
    CHECK_GL_ERROR(glAttachShader(program_, frag_shader_));

    GLint linked;
    CHECK_GL_ERROR(glLinkProgram(program_));
    glGetProgramiv(program_, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        LOGE("glLinkProgram failed");
        releaseProgram();
        return false;
    }

    return true;
}
void ShaderProgram2::releaseProgram()
{
    for(auto& item : attrib_storage_){
        auto& binder = item.second.gl_binder;
        if (enable_vbo_ && binder.buffer){
            glDeleteBuffers(1,&binder.buffer);
            binder.buffer = 0;
        }
        binder.location = -1;
    }
    for(auto& item : texture_storage_){
        auto& binder = item.second.gl_binder;
        if (enable_vbo_ && binder.buffer){
            glDeleteBuffers(1,&binder.buffer);
            binder.buffer = 0;
        }
        binder.location = -1;
    }
    glUseProgram(0);
    if (program_){
        glDeleteProgram(program_);
        program_ = 0;
    }
    if (vert_shader_){
        glDeleteShader(vert_shader_);
        vert_shader_ = 0;
    }
    if (frag_shader_){
        glDeleteShader(frag_shader_);
        frag_shader_ = 0;
    }
}

bool ShaderProgram2::use() {
    if (program_ == 0)
        return false;

    GLint currProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM,&currProgram);
    if (currProgram != program_) {
        CHECK_GL_ERROR(glUseProgram(program_));
        if(glGetError() != GL_NO_ERROR) {
            return false;
        }
    }
    return true;
}

bool ShaderProgram2::beginDraw()
{
    if(use()){
        //set blend mode and BlendFunc
        if (blend_func_ != BlendFunc::DISABLE) {
            glEnable(GL_BLEND);
            glBlendFunc(blend_func_.src, blend_func_.dst);
        }else
            glDisable(GL_BLEND);
        
        //set vertexattrib values
        for (auto &item : attrib_storage_) {
            prepareAttribs(item.second);
        }
        
        //set uniform values
        unsigned int texture_count = 0;
        for (auto &item : texture_storage_) {
            prepareTextures(item.second, texture_count);
        }
        return true;
    }else
        LOGE("ShaderProgram2::beginDraw failed..");
    return false;
}

GLint ShaderProgram2::GetUniformLocation(const char* key) {
    GLint location;
    CHECK_GL_ERROR(location = glGetUniformLocation(program_, key));
    return location;
}

GLuint ShaderProgram2::LoadShader(GLenum type, const char* shaderSrc) {

    GLuint shader = glCreateShader(type);
    if (!shader){
        return 0;
    }

    CHECK_GL_ERROR(glShaderSource(shader, 1, &shaderSrc, NULL));
    CHECK_GL_ERROR(glCompileShader(shader));
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
           LOGE("glCompileShader error: %s", info_log);
           free ( info_log );
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

int ShaderProgram2::drawIndices(DrawMode mode, int count)
{
    if (beginDraw()){
        const auto& indices = element_storage_.getVal<std::vector<int>>();
        CHECK_GL_ERROR(glDrawElements(getGlDrawMode(mode),count,GL_UNSIGNED_INT, indices.data()));
        return 0;
    }

    return -1;
}
int ShaderProgram2::draw(DrawMode mode, int count)
{
    if (beginDraw()){
        CHECK_GL_ERROR(glDrawArrays(getGlDrawMode(mode),0, count));
        return 0;
    }

    return -1;
}
int ShaderProgram2::drawRectangle()
{
    if (beginDraw()){
        CHECK_GL_ERROR(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
        return 0;
    }

    return -1;
}

GRAPHICCORE_END
