
#pragma once // include the current header file only once during a single compilation (translation unit)



#include<string>
#include<stdexcept>
#include<utility>

#include<GL/glew.h> 

#include"TextFile.h"



class VertexShader {
public:
    GLuint handle;


    VertexShader()
    : handle{}  //set to 0
    {}


    /*
    Constructs and compiles a vertex shader

    Throws std::runtime_error if the shader cannot be created or compilation fails.
    */
    explicit VertexShader(TextFile const& textFile) {
        handle = glCreateShader(GL_VERTEX_SHADER);
        if (!handle)
            throw std::runtime_error("glCreateShader for GL_VERTEX_SHADER failed");

        char const* sourceCode = textFile.content.c_str();  //from c++11 std::string is null-terminated
        glShaderSource(handle, 1, &sourceCode, nullptr);    //sourceCode has t obe null-terminated
        glCompileShader(handle);

        GLint compiled = GL_FALSE;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);
        if (GL_FALSE == compiled) {
            GLint logLength = 0;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(1 < logLength? logLength : 1,  ' ');
            glGetShaderInfoLog(handle, logLength, nullptr, &log[0]);    //will be null-terminated
            glDeleteShader(handle);
            throw std::runtime_error("VertexShader compilation failed:\n" + log);
        }
    }


    ~VertexShader() noexcept {
        glDeleteShader(handle);   //A value of 0 will be silently ignored
    }

    // Copy disabled
    VertexShader(VertexShader const&) = delete;
    VertexShader& operator=(VertexShader const&) = delete;


    VertexShader(VertexShader&& other) noexcept
    : handle(std::exchange(other.handle, 0)) //save other.handle in handle and change other.handle to 0
    {}


    VertexShader& operator=(VertexShader&& other) noexcept {
        std::swap(handle, other.handle);
        return *this;
    }
};