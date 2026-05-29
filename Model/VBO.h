#pragma once // include the current header file only once during a single compilation (translation unit)



#include<string>
#include<stdexcept>
#include<utility>
#include<vector>

#include<GL/glew.h>

#include<glm/vec3.hpp>



class VBO {
public:
    GLuint handle;


    VBO()
    : handle{}  //set to 0
    {}


    /*
    Creates and populates with data an opengl buffer
    The constructor can change bound opengl vertex buffer.
    */
    explicit VBO(std::vector<glm::vec3> const& elements)
    : handle{}  //set to 0
    {
        glGenBuffers(1, &handle);
        glBindBuffer(GL_ARRAY_BUFFER, handle);
        glBufferData(GL_ARRAY_BUFFER,
            elements.size() * sizeof(glm::vec3),
            elements.data(), GL_STATIC_DRAW);

        // Check for OpenGL errors
        GLenum const ERROR = glGetError();
        if (GL_NO_ERROR != ERROR) {
            glDeleteBuffers(1, &handle);
            throw std::runtime_error("VBO cannot be constructed: " + std::to_string(ERROR));
        }
    }


    ~VBO() noexcept {
        glDeleteBuffers(1, &handle); //A value of 0 will be silently ignored
    }


    // Copy disabled
    VBO(VBO const&) = delete;
    VBO& operator=(VBO const&) = delete;


    VBO(VBO&& other) noexcept
    : handle(std::exchange(other.handle, 0))
    {}


    VBO& operator=(VBO&& other) noexcept {
        std::swap(handle, other.handle);
        return *this;
    }


    //binds a buffer as part of current rendering state
    void use() const {
        glBindBuffer(GL_ARRAY_BUFFER, handle);
    }


    //unbinds any buffer previously bound
    static void useNone() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};