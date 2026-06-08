#pragma once // include the current header file only once during a single compilation (translation unit)



#include<GL/glew.h> 

#include"OBJFile.h"
#include"VBO.h"



struct ColoredMesh {
	GLuint vaoHandle;
    GLsizei vertexCount;
    VBO positions;
    VBO colors;
    VBO normals;


    ColoredMesh()
    : vaoHandle{}  //set to 0
    , vertexCount{}
    {}


	/*
	Converts a Wavefront OBJ file to a vertex array object used to render a 3d mesh
    The constructor can change bound opengl vertex buffer and vertex array.
	Throws std::runtime_error if an error occurred.
	*/
    explicit ColoredMesh(OBJFile const& objFile);


    ~ColoredMesh() noexcept {   //this destructor is called before destructors of each VBO
        glDeleteVertexArrays(1, &vaoHandle);   //A value of 0 will be silently ignored
    }


    // Copy disabled
    ColoredMesh(ColoredMesh const&) = delete;
    ColoredMesh& operator=(ColoredMesh const&) = delete;


    ColoredMesh(ColoredMesh&& other) noexcept
    //save other.vaoHandle in vaoHandle and change other.vaoHandle to 0
    : vaoHandle(std::exchange(other.vaoHandle, 0))
    , vertexCount(std::exchange(other.vertexCount, 0))
    , positions(std::exchange(other.positions, VBO{}))
    , colors(std::exchange(other.colors, VBO{}))
    , normals(std::exchange(other.normals, VBO{}))
    {}


    ColoredMesh& operator=(ColoredMesh&& other) noexcept {
        std::swap(vaoHandle, other.vaoHandle);
        std::swap(vertexCount, other.vertexCount);
        std::swap(positions, other.positions);
        std::swap(colors, other.colors);
        std::swap(normals, other.normals);
        return *this;
    }


    //bind a vertex array as part of current rendering state
    void use() const {
        glBindVertexArray(vaoHandle);
    }


    //unbinds any vertex array previously bound
    static void useNone() {
        glBindVertexArray(0);
    }
};
