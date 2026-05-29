#pragma once // include the current header file only once during a single compilation (translation unit)



#include<string>
#include<vector>

#include<glm/vec3.hpp>



/*
Wavefront OBJ files containing 3D models (can be exported from Blender).
After loading, the data is expanded: each triangle corner has its own
copy of position, color, and normal (no separate index buffers).
*/
class OBJFile {
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;


    /*
    Loads a file from disk.
    Throws std::runtime_error if the file cannot be loaded or parsed.
    */
    static OBJFile fromDisk(std::string const& filePath);
};