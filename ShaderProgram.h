#pragma once // include the current header file only once during a single compilation (translation unit)



#include<string>
#include<stdexcept>
#include<utility>

#include<GL/glew.h>

#include"VertexShader.h"
#include"FragmentShader.h"



class ShaderProgram {
public:
    GLuint handle;


    ShaderProgram()
    : handle{}  //set to 0
    {}


    /*
    Constructs and links an OpenGL program from a vertex and fragment shader.

    Throws std::runtime_error if the program cannot be created or the link operation fails.
    */
    ShaderProgram(VertexShader const& vs,  FragmentShader const& fs) {
        handle = glCreateProgram();
        if (!handle)
            throw std::runtime_error("glCreateProgram failed");

        glAttachShader(handle, vs.handle);
        glAttachShader(handle, fs.handle);
        glLinkProgram(handle);

        GLint linked = GL_FALSE;
        glGetProgramiv(handle, GL_LINK_STATUS, &linked);
        if (GL_FALSE == linked) {
            GLint logLength = 0;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLength);
            std::string log(1 < logLength ? logLength : 1,  ' ');
            glGetProgramInfoLog(handle, logLength, nullptr, &log[0]); //will be null-terminated
            glDeleteProgram(handle);    //vs and fs are automatically detached
            throw std::runtime_error("ShaderProgram linking failed:\n" + log);
        }

        //Shaders are no longer needed after linking – detach them
        glDetachShader(handle, vs.handle);
        glDetachShader(handle, fs.handle);
    }


    ~ShaderProgram() noexcept {
        glDeleteProgram(handle); //A value of 0 will be silently ignored
    }


    // Copy disabled
    ShaderProgram(ShaderProgram const&) = delete;
    ShaderProgram& operator=(ShaderProgram const&) = delete;


    ShaderProgram(ShaderProgram&& other) noexcept
    : handle(std::exchange(other.handle, 0))
    {}


    ShaderProgram& operator=(ShaderProgram&& other) noexcept {
        std::swap(handle, other.handle);
        return *this;
    }


    //Installs a program object as part of current rendering state
    void use() const {
        glUseProgram(handle);
    }


    //deactivate any shader program
    static void useNone() {
        glUseProgram(0);
    }
};