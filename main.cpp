#include<GL/glew.h> //we have to include this before any other opengl headers

#include<GLFW/glfw3.h>

#include<glm/glm.hpp>             
#include<glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include<exception>
#include<stdexcept>
#include<algorithm>
#include<vector>
#include<cmath>

#include"TextFile.h"
#include"VertexShader.h"
#include"FragmentShader.h"
#include"ShaderProgram.h"
#include"OBJFile.h"
#include"ColoredMesh.h"



using std::cerr, std::cout;



float cameraYRadians = 0.0f;

// initial position
glm::vec3 ufoPosition{ -3.0f, 4.0f, 0.0f };   

// velocity vector (units per second)
glm::vec3 ufoVelocity{ 0.0f, 0.0f, 0.0f };  
float const UFO_SPEED = 2.0f;



void keyCallback(GLFWwindow* window,  int key,  int scancode,  int action,  int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
        ufoVelocity = glm::vec3(0.0f, 0.0f, -UFO_SPEED);
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        ufoVelocity = glm::vec3(0.0f, 0.0f, UFO_SPEED);
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
        ufoVelocity = glm::vec3(-UFO_SPEED, 0.0f, 0.0f);
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        ufoVelocity = glm::vec3(UFO_SPEED, 0.0f, 0.0f);

    else if (key == GLFW_KEY_LEFT)
        cameraYRadians = std::max(-1.0f, cameraYRadians-0.1f);
    else if (key == GLFW_KEY_RIGHT)
        cameraYRadians = std::min(1.0f, cameraYRadians+0.1f);
}



void errorCallback(int error,  const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}



int main() {
    try {
        glfwSetErrorCallback(errorCallback);

        if (!glfwInit())
            throw std::runtime_error{"glfwInit falied"};

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWwindow* window = glfwCreateWindow(1024, 768, "Sheep", NULL, NULL);
        if (!window) {
            glfwTerminate();
            throw std::runtime_error{"glfwCreateWindow falied"};
        }

        glfwSetKeyCallback(window, keyCallback);
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;

        if (glewInit() != GLEW_OK) {
            glfwDestroyWindow(window);
            glfwTerminate();
            throw std::runtime_error{ "glewInit falied" };
        }

        GLint glMajor;
        glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
        if (glMajor < 3) {
            glfwDestroyWindow(window);
            glfwTerminate();
            throw std::runtime_error{"glMajor < 3"};
        }

        TextFile vertexSource{"colorNormal.v"};
        TextFile fragmentSource{ "colorNormal.f" };
        VertexShader vertexShader(vertexSource);
        FragmentShader fragmentShader(fragmentSource);
        ShaderProgram shaderProgram{vertexShader, fragmentShader};
        OBJFile terrain = OBJFile::fromDisk("terrain.obj");
        ColoredMesh terrainMesh{ terrain };
        OBJFile sheep = OBJFile::fromDisk("sheep.obj");
        ColoredMesh sheepMesh{ sheep };
        OBJFile tree1 = OBJFile::fromDisk("tree1.obj");
        ColoredMesh tree1Mesh{ tree1 };
        OBJFile ufo = OBJFile::fromDisk("ufo.obj");
        ColoredMesh ufoMesh{ ufo };
        std::vector<glm::vec3> positions{
            glm::vec3{0.0f,0.0f,0.0f}, glm::vec3{-0.5f,1.8f,3.9f}, glm::vec3{-6.0f,1.0f,5.0f }};
        std::vector<ColoredMesh*> meshes{&terrainMesh, &sheepMesh, &tree1Mesh};

        GLint mvpLoc = glGetUniformLocation(shaderProgram.handle, "modelViewProjMatrix");
        GLint normalMatLoc = glGetUniformLocation(shaderProgram.handle, "normalMatrix");
        GLint lightDirLoc = glGetUniformLocation(shaderProgram.handle, "vectorToLight");
        GLint ambientLoc = glGetUniformLocation(shaderProgram.handle, "sceneAmbient");
        float background = 0.0f;
        float backgroundChangeSpeed = 1.0f;
        glm::mat4 modelMatrix;
        glm::mat3 normalMatrix;
        glm::mat4 mvpMatrix;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        double lastSeconds = glfwGetTime();
        while (!glfwWindowShouldClose(window)) {
            double const CURRENT_SECONDS = glfwGetTime();
            //with min we avoid huge step if the game freezes
            float const STEP_SECONDS = std::min(0.33f, (float)(CURRENT_SECONDS - lastSeconds));
            lastSeconds = CURRENT_SECONDS;

            ufoPosition += ufoVelocity * STEP_SECONDS;

            background += backgroundChangeSpeed * STEP_SECONDS;
            if (1.0f < background) {
                background = 1.0f;
                backgroundChangeSpeed *= -1.0f;
            }
            else if (background < 0.0f) {
                background = 0.0f;
                backgroundChangeSpeed *= -1.0f;
            }

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);

            glClearColor(background, background, background, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //clear color and depth

            float const ASPECT = width / (float)height;
            glm::mat4 const PROJ_MATRIX = glm::perspective(glm::radians(45.0f), ASPECT, 0.1f, 100.0f);
            //dirction to the light (directional light) in world space
            glm::vec3 const TO_LIGHT_DIR = glm::normalize(glm::vec3(0.0f, 0.25f, 1.0f)); 
            glm::vec3 const AMBIENT = glm::vec3(0.05f, 0.05f, 0.05f);
            float const UFO_ROT_RADIANS = (float)glfwGetTime() * 0.5f;  // 0.5 radians per second
            glm::vec3 const CAMERA_POS(12.0f * sin(cameraYRadians),  6.0f,  12.0f * cos(cameraYRadians));
            glm::vec3 const CAMERA_TARGET(0.0f, 0.0f, 0.0f);
            glm::vec3 const CAMERA_UP(0.0f, 1.0f, 0.0f);
            glm::mat4 const VIEW_MATRIX = glm::lookAt(CAMERA_POS, CAMERA_TARGET, CAMERA_UP);

            shaderProgram.use();
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(TO_LIGHT_DIR));
            glUniform3fv(ambientLoc, 1, glm::value_ptr(AMBIENT));

            for (std::size_t i = 0;  i < meshes.size();  ++i) {
                meshes[i]->use();                 // binds the VAO
                modelMatrix = glm::translate(glm::mat4(1.0f), positions[i]);
                //only left-upper 3x3 portion is needed for normal vectors
                normalMatrix = glm::transpose(glm::inverse(modelMatrix));
                mvpMatrix = PROJ_MATRIX * VIEW_MATRIX * modelMatrix;
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
                glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
                glDrawArrays(GL_TRIANGLES, 0, meshes[i]->vertexCount);
            }

            ufoMesh.use();                 // binds the VAO
            modelMatrix = glm::translate(glm::mat4(1.0f), ufoPosition);
            //multiply by a rotation matrix (Y axis) on the right side
            modelMatrix = glm::rotate(modelMatrix, 2*UFO_ROT_RADIANS, glm::vec3(0.0f,1.0f,0.0f));
            //only left-upper 3x3 portion is needed for normal vectors
            normalMatrix = glm::transpose(glm::inverse(modelMatrix));
            mvpMatrix = PROJ_MATRIX * VIEW_MATRIX * modelMatrix;
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
            glDrawArrays(GL_TRIANGLES, 0, ufoMesh.vertexCount);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        //glfwDestroyWindow(window);    //we exit so will be automatically cleared
        //glfwTerminate();              //we exit so will be automatically cleared
        return 0;
    } catch (std::exception const& ex) {
        cerr << "ERROR: " << ex.what() << '\n';
        return 1;
    }
}