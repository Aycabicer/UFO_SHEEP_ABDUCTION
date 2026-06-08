#pragma once // include the current header file only once during a single compilation (translation unit)



#include<string>
#include<stdexcept>
#include<utility>

#include<GL/glew.h>

#include"TextFile.h"



class FragmentShader {
public:
    GLuint handle;


    FragmentShader()
    : handle{}  //set to 0
    {}


    /*
    Constructs and compiles a fragment shader

    Throws std::runtime_error if the shader cannot be created or compilation fails.
    */
    explicit FragmentShader(TextFile const& textFile) {
        handle = glCreateShader(GL_FRAGMENT_SHADER);
        if (!handle)
            throw std::runtime_error("glCreateShader for GL_FRAGMENT_SHADER failed");

        char const* sourceCode = textFile.content.c_str();  //from c++11 std::string is null-terminated
        glShaderSource(handle, 1, &sourceCode, nullptr);    //sourceCode has t obe null-terminated
        glCompileShader(handle);

        GLint compiled = GL_FALSE;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);
        if (GL_FALSE == compiled) {
            GLint logLength = 0;
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(1 < logLength ? logLength : 1, ' ');
            glGetShaderInfoLog(handle, logLength, nullptr, &log[0]);    //will be null-terminated
            glDeleteShader(handle);
            throw std::runtime_error("FragmentShader compilation failed:\n" + log);
        }
    }


    ~FragmentShader() noexcept {
        glDeleteShader(handle);   //A value of 0 will be silently ignored
    }

    // Copy disabled
    FragmentShader(FragmentShader const&) = delete;
    FragmentShader& operator=(FragmentShader const&) = delete;


    FragmentShader(FragmentShader&& other) noexcept
    : handle(std::exchange(other.handle, 0)) //save other.handle in handle and change other.handle to 0
    {}


    FragmentShader& operator=(FragmentShader&& other) noexcept {
        std::swap(handle, other.handle);
        return *this;
    }
};
