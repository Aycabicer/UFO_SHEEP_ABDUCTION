#include"OBJFile.h"

#include<stdexcept>

#include<cstdio>
#include<cstring>



//a basic loader for wavefront obj files (can be exported from Blender)
OBJFile OBJFile::fromDisk(std::string const& filePath) {
    OBJFile objFile;
    std::vector<glm::vec3> allPositions;      // positions as they appear
    std::vector<glm::vec3> allColors;         // colors per vertex (same indexing as positions)
    std::vector<glm::vec3> allNormals;        // normals as they appear

    FILE* diskFile = fopen(filePath.c_str(), "r");
    if (!diskFile)
        throw std::runtime_error("Could not open '" + filePath + "'");

    char lineHeader[256];
    while (true) {
        //Matches up to width or until the first whitespace character, whichever appears first
        if (EOF == fscanf(diskFile, "%255s", lineHeader)) //255 - we need space for the null-character
            break;

        //position
        if (0 == strcmp(lineHeader, "v")) {
            glm::vec3 pos;
            glm::vec3 col;
            // Try to read 6 floats: x y z r g b
            int const READ_FIELDS = fscanf(diskFile, "%f %f %f %f %f %f",
                &pos.x, &pos.y, &pos.z,
                &col.x, &col.y, &col.z);
            if (6 != READ_FIELDS) {
                fclose(diskFile);
                throw std::runtime_error("Invalid vertex in " + filePath);
            }
            allPositions.push_back(pos);
            allColors.push_back(col);
        //normal
        } else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            int const READ_FIELDS = fscanf(diskFile, "%f %f %f", &normal.x, &normal.y, &normal.z);
            if (3 != READ_FIELDS) {
                fclose(diskFile);
                throw std::runtime_error("Invalid normal in " + filePath);
            }
            allNormals.push_back(normal);
        //triangles
        } else if (0 == strcmp(lineHeader, "f")) {
            // Indices: position//normal
            int p1, n1, p2, n2, p3, n3; //one-based indices
            int const READ_FIELDS = fscanf(diskFile, "%d//%d %d//%d %d//%d",
                &p1, &n1,
                &p2, &n2,
                &p3, &n3);
            if (6 != READ_FIELDS) {
                fclose(diskFile);
                throw std::runtime_error("Invalid triangle in " + filePath);
            }
            // Convert to zero-based indices
            --p1; --n1;
            --p2; --n2;
            --p3; --n3;
            if (allPositions.size() <= p1  || allPositions.size() <= p2  || allPositions.size() <= p3) {
                fclose(diskFile);
                throw std::runtime_error("Invalid position index in " + filePath);
            }
            if (allNormals.size() <= n1 || allNormals.size() <= n2 || allNormals.size() <= n3) {
                fclose(diskFile);
                throw std::runtime_error("Invalid normal index in " + filePath);
            }

            //for each triangle corner, append position, color, normal

            objFile.positions.push_back(allPositions[p1]);
            objFile.colors.push_back(allColors[p1]);      // color uses same index as position
            objFile.normals.push_back(allNormals[n1]);

            objFile.positions.push_back(allPositions[p2]);
            objFile.colors.push_back(allColors[p2]);      // color uses same index as position
            objFile.normals.push_back(allNormals[n2]);

            objFile.positions.push_back(allPositions[p3]);
            objFile.colors.push_back(allColors[p3]);      // color uses same index as position
            objFile.normals.push_back(allNormals[n3]);
        } else //we don't need this line
            fscanf(diskFile, "%*[^\n]"); //Read and discard everything until a newline (\n) is found
    }

    fclose(diskFile);
    return objFile;
}