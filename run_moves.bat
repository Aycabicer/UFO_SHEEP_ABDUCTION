@echo off
cd /d "C:\Users\aycab\OneDrive\Masaüstü\UFO_Sheep_Abduction"

echo Running command 1: git mv sheep5/OBJFile.h Model/OBJFile.h
git mv sheep5/OBJFile.h Model/OBJFile.h

echo Running command 2: git mv sheep5/OBJFile.cpp Model/OBJFile.cpp
git mv sheep5/OBJFile.cpp Model/OBJFile.cpp

echo Running command 3: git mv sheep5/ColoredMesh.h Model/ColoredMesh.h
git mv sheep5/ColoredMesh.h Model/ColoredMesh.h

echo Running command 4: git mv sheep5/ColoredMesh.cpp Model/ColoredMesh.cpp
git mv sheep5/ColoredMesh.cpp Model/ColoredMesh.cpp

echo Running command 5: git mv sheep5/VBO.h Model/VBO.h
git mv sheep5/VBO.h Model/VBO.h

echo Running command 6: git mv sheep5/TextFile.h TextFile.h
git mv sheep5/TextFile.h TextFile.h

echo Running command 7: git mv sheep5/colorNormal.v colorNormal.v
git mv sheep5/colorNormal.v colorNormal.v

echo Running command 8: git mv sheep5/colorNormal.f colorNormal.f
git mv sheep5/colorNormal.f colorNormal.f

echo Running command 9: git mv sheep5/sheep.obj Sheep/sheep.obj
git mv sheep5/sheep.obj Sheep/sheep.obj

echo Running command 10: git mv sheep5/ufo.obj UFO/ufo.obj
git mv sheep5/ufo.obj UFO/ufo.obj

echo Running command 11: git mv sheep5/terrain.obj Ground/terrain.obj
git mv sheep5/terrain.obj Ground/terrain.obj

echo Running command 12: git mv sheep5/tree1.obj Scene/tree1.obj
git mv sheep5/tree1.obj Scene/tree1.obj

echo Running command 13: mkdir "C:\Users\aycab\OneDrive\Masaüstü\UFO_Sheep_Abduction\Shader"
mkdir "C:\Users\aycab\OneDrive\Masaüstü\UFO_Sheep_Abduction\Shader"

echo Running command 14: git mv sheep5/VertexShader.h Shader/VertexShader.h
git mv sheep5/VertexShader.h Shader/VertexShader.h

echo Running command 15: git mv sheep5/FragmentShader.h Shader/FragmentShader.h
git mv sheep5/FragmentShader.h Shader/FragmentShader.h

echo Running command 16: git mv sheep5/ShaderProgram.h Shader/ShaderProgram.h
git mv sheep5/ShaderProgram.h Shader/ShaderProgram.h

echo Running command 17: git mv sheep5/glm glm
git mv sheep5/glm glm

echo Running command 18: git mv sheep5/sheep Sheep/sheep
git mv sheep5/sheep Sheep/sheep

echo Running command 19: git rm sheep5/main.cpp
git rm sheep5/main.cpp

echo All commands completed!
