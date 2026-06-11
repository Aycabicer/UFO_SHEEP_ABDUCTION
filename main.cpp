#include<GL/glew.h> //we have to include this before any other opengl headers

#include<GLFW/glfw3.h>

#include<glm/glm.hpp>             
#include<glm/gtc/matrix_transform.hpp> 
#include<glm/gtc/type_ptr.hpp>

#include<iostream>
#include<exception>
#include<stdexcept>
#include<vector>
#include<algorithm>
#include<utility>

#include<cmath>

#include"TextFile.h"
#include"VertexShader.h"
#include"FragmentShader.h"
#include"ShaderProgram.h"
#include"OBJFile.h"
#include"ColoredMesh.h"



using std::cerr, std::cout;


/*
g++ -std=c++17 main.cpp Model/OBJFile.cpp Model/ColoredMesh.cpp -o app_mac -I. -IModel -IShader -Iglm -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -lGLEW -framework OpenGL && ./app_mac
*/


enum class UfoState {
    MOVING,
    FIND_SHEEP,
    BEAM_DOWN,
    SHEEP_UP,
    BEAM_UP
};



int const UFO_ID = 0;
int const FIRST_SHEEP_ID = 4;
float const INF = 1000000.0f;
UfoState ufoState = UfoState::MOVING;
float beamScaleYStep = 2.0f;        //3 units per second
float closestSheepDistanceXZ = INF; ///we only check distances to sheep on the xz plane
float closesSheepDistanceY = INF;





//for each position and scale we have to add appropriate 3d mesh in meshes (defined in main)
std::vector<glm::vec3> positions{
    glm::vec3{0.0f, 4.2f, 0.0f},      //ufo
    glm::vec3{0.0f,0.0f,0.0f},        //terrain
    glm::vec3{-5.7f, 1.7, -6.2},      //tree
    glm::vec3{-3.8f, 0.9f, 5.7f},     //tree
    glm::vec3{2.7f, 1.84f, 4.38f},    //sheep
    glm::vec3{-0.94f, 1.5f, 5.8f},    //sheep
    glm::vec3{2.3f, 1.05f, 6.3f},     //sheep
    glm::vec3{0.0f, 0.0f, 0.0f}};     //beam is always last
std::vector<glm::vec3> scale{
    glm::vec3{1.0f, 1.0f, 1.0f},        //ufo
    glm::vec3{1.0f, 1.0f, 1.0f},        //terrain
    glm::vec3{0.5f, 0.5f, 0.5f},        //tree
    glm::vec3{0.75f, 0.75f, 0.75f},     //tree
    glm::vec3{0.75f, 0.75f, 0.75f},     //sheep
    glm::vec3{1.0f, 1.0f, 1.0f},        //sheep
    glm::vec3{0.5f, 0.5f, 0.5f},        //sheep
    glm::vec3{1.0f, 0.0f, 1.0f} };      //beam is always last
std::vector<float> alpha{
    1.0f,   //ufo
    1.0f,   //terrain
    1.0f,   //tree
    1.0f,   //tree
    1.0f,   //sheep
    1.0f,   //sheep
    1.0f,   //sheep
    0.4f};  //beam is always last

float cameraYRadians = 0.0f;

// velocity vector (units per second)
glm::vec3 ufoVelocity{ 0.0f, 0.0f, 0.0f };  
float const UFO_SPEED = 2.0f;



void keyCallback(GLFWwindow* window,  int key,  int scancode,  int action,  int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    else if (key == GLFW_KEY_LEFT)
        cameraYRadians = std::max(-1.0f, cameraYRadians - 0.1f);
    else if (key == GLFW_KEY_RIGHT)
        cameraYRadians = std::min(1.0f, cameraYRadians + 0.1f);
    else if (ufoState == UfoState::MOVING) {


        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
            ufoVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
            std::size_t const BEAM_ID =positions.size() - 1; //beam is always last
            scale[BEAM_ID].y = 0.0f;
            closestSheepDistanceXZ = INF;
            closesSheepDistanceY = INF;
            ufoState = UfoState::FIND_SHEEP;
        } else if (key == GLFW_KEY_W && action == GLFW_PRESS)
            ufoVelocity = glm::vec3(0.0f, 0.0f, -UFO_SPEED);
        else if (key == GLFW_KEY_S && action == GLFW_PRESS)
            ufoVelocity = glm::vec3(0.0f, 0.0f, UFO_SPEED);
        else if (key == GLFW_KEY_A && action == GLFW_PRESS)
            ufoVelocity = glm::vec3(-UFO_SPEED, 0.0f, 0.0f);
        else if (key == GLFW_KEY_D && action == GLFW_PRESS)
            ufoVelocity = glm::vec3(UFO_SPEED, 0.0f, 0.0f);
    }
}



void errorCallback(int error,  const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}



void updateState(float stepSeconds,  std::vector<ColoredMesh*>& meshes) {
    if (UfoState::FIND_SHEEP == ufoState) {
        int closestSheepId = -1;
        std::size_t const LAST_SHEEP_ID = positions.size() - 2; // beam is always last
        for (int i=FIRST_SHEEP_ID; i<=LAST_SHEEP_ID; ++i) {
            glm::vec3 const VEC = positions[UFO_ID] - positions[i]; //from sheep to Ufo
            //we only check distances on the xz plane
            float const DISTANCE = std::sqrtf(VEC.x * VEC.x + VEC.z * VEC.z);
            if (DISTANCE < closestSheepDistanceXZ) {
                closestSheepDistanceXZ = DISTANCE;
                closesSheepDistanceY = VEC.y;
                closestSheepId = i;
            }
        }
        /*
        When we have one sheep, we position it as the last sheep,
        so that all opaque objects are displayed first
        */
        if (INF != closestSheepDistanceXZ) {
            std::swap(positions[LAST_SHEEP_ID], positions[closestSheepId]);
            std::swap(scale[LAST_SHEEP_ID], scale[closestSheepId]);
            std::swap(alpha[LAST_SHEEP_ID], alpha[closestSheepId]);
            std::swap(meshes[LAST_SHEEP_ID], meshes[closestSheepId]);
        } else { //no sheep, so slightly show a beam
            closestSheepDistanceXZ = INF;
            closesSheepDistanceY = 1.0f;    
        }
        ufoState = UfoState::BEAM_DOWN;
    } else if (UfoState::BEAM_DOWN == ufoState) {
        std::size_t const BEAM_ID = positions.size() - 1;
        scale[BEAM_ID].y += beamScaleYStep * stepSeconds;    //scale the beam so that it reaches sheep
        if (closesSheepDistanceY <= scale[BEAM_ID].y) {
            scale[BEAM_ID].y = closesSheepDistanceY;
            if (closestSheepDistanceXZ < 0.5f)  //is the beam close enough?
                ufoState = UfoState::SHEEP_UP;
            else
                ufoState = UfoState::BEAM_UP;
        }
    } else if (UfoState::BEAM_UP == ufoState) {
        std::size_t const BEAM_ID = positions.size() - 1;
        scale[BEAM_ID].y -= beamScaleYStep * stepSeconds; ;    //scale the beam so that it goes back to UFO
        if (scale[BEAM_ID].y <= 0.0f) {
            scale[BEAM_ID].y = 0.0f;
            ufoState = UfoState::MOVING;
        }
    } else if (UfoState::SHEEP_UP == ufoState) {
        std::size_t const LAST_SHEEP_ID = positions.size() - 2;
        std::size_t const BEAM_ID = positions.size() - 1;
        closesSheepDistanceY -= beamScaleYStep * stepSeconds;            //diminish distance over time
        alpha[LAST_SHEEP_ID] -= 0.5f * beamScaleYStep * stepSeconds;    //diminish alpha over time
        alpha[LAST_SHEEP_ID] = std::max(0.0f, alpha[LAST_SHEEP_ID]);     //no negative values allowed
        if (closesSheepDistanceY <= 0.0f) {
            //place sheep at the end and remove
            std::swap(positions[LAST_SHEEP_ID], positions[BEAM_ID]);
            std::swap(scale[LAST_SHEEP_ID], scale[BEAM_ID]);
            std::swap(alpha[LAST_SHEEP_ID], alpha[BEAM_ID]);
            std::swap(meshes[LAST_SHEEP_ID], meshes[BEAM_ID]);
            positions.pop_back();
            scale.pop_back();
            alpha.pop_back();
            meshes.pop_back();
            ufoState = UfoState::BEAM_UP;
        } else {
            positions[LAST_SHEEP_ID].x = positions[UFO_ID].x;
            positions[LAST_SHEEP_ID].y = positions[UFO_ID].y - closesSheepDistanceY;
            positions[LAST_SHEEP_ID].z = positions[UFO_ID].z;
        }
    }
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

        //Delta time /Frame-indep.
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
        OBJFile cone = OBJFile::fromDisk("cone.obj");
        ColoredMesh coneMesh{cone};
        OBJFile moon = OBJFile::fromDisk("moon.obj");
        ColoredMesh moonMesh{ moon };
        
        std::vector<ColoredMesh*> meshes{&ufoMesh, &terrainMesh, &tree1Mesh, &tree1Mesh,
            &sheepMesh, &sheepMesh, &sheepMesh, &coneMesh};

        GLint mvpLoc = glGetUniformLocation(shaderProgram.handle, "modelViewProjMatrix");
        GLint normalMatLoc = glGetUniformLocation(shaderProgram.handle, "normalMatrix");
        GLint lightDirLoc = glGetUniformLocation(shaderProgram.handle, "vectorToLight");
        GLint ambientLoc = glGetUniformLocation(shaderProgram.handle, "sceneAmbient");
        GLint alphaLoc = glGetUniformLocation(shaderProgram.handle, "alpha");
        float background = 0.0f;
        float backgroundChangeSpeed = 1.0f;
        glm::mat4 modelMatrix;
        glm::mat3 normalMatrix;
        glm::mat4 mvpMatrix;

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        /*
        (alpha of a new fragment) * (color of a new fragment)
            + (1 - alpha of a new fragment) * (already existing color in the framebuffer)
        */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        double lastSeconds = glfwGetTime();
        while (!glfwWindowShouldClose(window)) {
            float const CURRENT_SECONDS = (float)glfwGetTime();
            //with min we avoid huge step if the game freezes
            float const STEP_SECONDS = std::min(0.33f, (float)(CURRENT_SECONDS - lastSeconds));
            lastSeconds = CURRENT_SECONDS;

            positions[UFO_ID] += ufoVelocity * STEP_SECONDS;

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

            //glClearColor(background, background, background, 1.0f);
            glClearColor(0.02f, 0.02f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //clear color and depth





            float const ASPECT = width / (float)height;
            glm::mat4 const PROJ_MATRIX = glm::perspective(glm::radians(45.0f), ASPECT, 0.1f, 100.0f);
            //dirction to the light (directional light) in world space
            //glm::vec3 const TO_LIGHT_DIR = glm::normalize(glm::vec3(0.0f, 0.25f, 1.0f)); 
            //glm::vec3 moonPos = glm::vec3(5.0f * sin(cameraYRadians + 1.5f), 8.0f, 5.0f * cos(cameraYRadians + 1.5f));
            //glm::vec3 moonPos = glm::vec3(8.0f, 6.0f, -8.0f);
            glm::vec3 moonPos = glm::vec3(8.0f, 3.0f, -8.0f);
            glm::vec3 const TO_LIGHT_DIR = glm::normalize(moonPos);
            //glm::vec3 const AMBIENT = glm::vec3(0.05f, 0.05f, 0.05f);
            glm::vec3 const AMBIENT = glm::vec3(0.3f, 0.3f, 0.4f);
            float const UFO_ROT_RADIANS = CURRENT_SECONDS;  // 1 radian per second
            glm::vec3 const CAMERA_POS(12.0f * sin(cameraYRadians),  6.0f,  12.0f * cos(cameraYRadians));
            glm::vec3 const CAMERA_TARGET(0.0f, 0.0f, 0.0f);
            //beam ufoya kitli
            glm::vec3 const CAMERA_UP(0.0f, 1.0f, 0.0f);
            glm::mat4 const VIEW_MATRIX = glm::lookAt(CAMERA_POS, CAMERA_TARGET, CAMERA_UP);

            //we display all opaque objects first
            shaderProgram.use();


// Yıldızlar
            srand(42);
            glUniform3fv(ambientLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
            glUniform1f(alphaLoc, 1.0f);
            glDisable(GL_DEPTH_TEST);
            moonMesh.use();
            for (int s = 0; s < 150; s++) {
                float angle = ((float)(rand() % 628)) / 100.0f;
                float elevAngle = ((float)(rand() % 80) + 5) * 3.14159f / 180.0f;

                //matrix concatenation
                float r = 12.0f;
                float sx = CAMERA_POS.x + r * cos(elevAngle) * cos(angle);

                float sy = CAMERA_POS.y + r * sin(elevAngle);

                float sz = CAMERA_POS.z + r * cos(elevAngle) * sin(angle);
                //Transpose inverse norm. matrix 
                glm::mat4 starModel = glm::translate(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
                starModel = glm::scale(starModel, glm::vec3(0.04f));
                glm::mat4 starMVP = PROJ_MATRIX * VIEW_MATRIX * starModel;
                glm::mat3 starNormal = glm::mat3(1.0f);
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(starMVP));
                glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(starNormal));
                glDrawArrays(GL_TRIANGLES, 0, moonMesh.vertexCount);
            }
            glEnable(GL_DEPTH_TEST);
            glUniform3fv(ambientLoc, 1, glm::value_ptr(AMBIENT));
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(TO_LIGHT_DIR));


            // Ay çiz
            glm::mat4 moonModel = glm::translate(glm::mat4(1.0f), moonPos);
            moonModel = glm::scale(moonModel, glm::vec3(0.8f));
            glm::mat4 moonMVP = PROJ_MATRIX * VIEW_MATRIX * moonModel;
            glm::mat3 moonNormal = glm::mat3(1.0f);
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(moonMVP));
            glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(moonNormal));
            glUniform1f(alphaLoc, 1.0f);
            glUniform3fv(ambientLoc, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.9f)));
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::vec3(0.0f, 1.0f, 0.0f)));
            moonMesh.use();
            glDrawArrays(GL_TRIANGLES, 0, moonMesh.vertexCount);
            glUniform3fv(ambientLoc, 1, glm::value_ptr(AMBIENT));
            glUniform3fv(lightDirLoc, 1, glm::value_ptr(TO_LIGHT_DIR));





            glUniform3fv(lightDirLoc, 1, glm::value_ptr(TO_LIGHT_DIR));
            glUniform3fv(ambientLoc, 1, glm::value_ptr(AMBIENT));

            // Gölge matrisi hesapla
            //glm::vec3 lightPos = glm::vec3(5.0f, 10.0f, 5.0f);
            //glm::vec3 lightPos = glm::vec3(0.0f, 0.25f, 1.0f);
            //glm::vec3 lightPos = glm::vec3(0.0f, 8.0f, 3.0f);
            //glm::vec3 lightPos = glm::vec3(0.0f, 20.0f, 10.0f);

            //glm::vec3 lightPos = CAMERA_POS * 2.0f;
            //lightPos.y = 20.0f;
            /*glm::mat4 shadowMat = Tinv * Mpersp * T;
            glm::vec3 lightPos = moonPos;
            lightPos.y = 6.0f;
            //glm::mat4 yOffset = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.3f, 0.0f));
            glm::mat4 T = glm::translate(glm::mat4(1.0f), -lightPos);
            glm::mat4 Mpersp = glm::mat4(1.0f);
            Mpersp[1][3] = -1.0f / lightPos.y;
            glm::mat4 Tinv = glm::translate(glm::mat4(1.0f), lightPos);
            //glm::mat4 shadowMat = Tinv * Mpersp * T;
            glm::mat4 shadowMat = yOffset * Tinv * Mpersp * T;*/
            glm::vec3 lightPos = moonPos;
            lightPos.y = 3.0f;
            glm::mat4 T = glm::translate(glm::mat4(1.0f), -lightPos);
            glm::mat4 Mpersp = glm::mat4(1.0f);
            Mpersp[1][3] = -1.0f / lightPos.y;
            glm::mat4 Tinv = glm::translate(glm::mat4(1.0f), lightPos);
            glm::mat4 shadowMat = Tinv * Mpersp * T;



            for (std::size_t i = 0;  i < meshes.size();  ++i) {
                std::size_t const BEAM_ID = positions.size() - 1;
                if (BEAM_ID == i)
                    positions[i] = positions[UFO_ID];    //beam has the same postion as UFO

                if (BEAM_ID != i  &&  1.0f != alpha[i])
                    glDisable(GL_DEPTH_TEST);   //always draw transparent objects
                glUniform1f(alphaLoc, alpha[i]);

                meshes[i]->use();                 // binds the VAO
                modelMatrix = glm::translate(glm::mat4(1.0f), positions[i]);    //translation is applied last
                if (i == UFO_ID)//multiply by a rotation matrix (Y axis) on the right side
                    modelMatrix = glm::rotate(modelMatrix, UFO_ROT_RADIANS, glm::vec3(0.0f, 1.0f, 0.0f));
                //multiply by a scaling matrix on the right side
                modelMatrix = glm::scale(modelMatrix, scale[i]);
                //only left-upper 3x3 portion is needed for normal vectors
                normalMatrix = glm::transpose(glm::inverse(modelMatrix));
                mvpMatrix = PROJ_MATRIX * VIEW_MATRIX * modelMatrix;
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
                glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
                glDrawArrays(GL_TRIANGLES, 0, meshes[i]->vertexCount);

                // Gölge çiz
                if (i >= 2 && i != BEAM_ID) {
                    
                    glm::vec3 objPos = positions[i];
                    glm::vec3 shadowPos;

                    if (ufoState == UfoState::SHEEP_UP && i == positions.size() - 2) {
                        shadowPos = glm::vec3(positions[UFO_ID].x, objPos.y - 0.1f, positions[UFO_ID].z);
                    } else {
                        glm::vec3 toLight = glm::normalize(lightPos - objPos);
                        glm::vec3 shadowOffset = glm::vec3(-toLight.x, 0.0f, -toLight.z) * 0.3f;
                        shadowPos = glm::vec3(objPos.x + shadowOffset.x, objPos.y - 0.1f, objPos.z + shadowOffset.z);
                    }

                    glm::mat4 shadowModel = glm::translate(glm::mat4(1.0f), shadowPos);
                    shadowModel = glm::scale(shadowModel, glm::vec3(scale[i].x, 0.01f, scale[i].z));


                    glm::mat4 shadowMVP = PROJ_MATRIX * VIEW_MATRIX * shadowModel;
                    glm::mat3 shadowNormal = glm::mat3(1.0f);
                    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP));
                    glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(shadowNormal));
                    glUniform1f(alphaLoc, 0.4f);
                    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
                    //glUniform3fv(ambientLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
                    glUniform3fv(ambientLoc, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
                    //glDisable(GL_DEPTH_TEST);
                    glDrawArrays(GL_TRIANGLES, 0, meshes[i]->vertexCount);
                    //glEnable(GL_DEPTH_TEST);
                    glUniform3fv(lightDirLoc, 1, glm::value_ptr(TO_LIGHT_DIR));
                    glUniform3fv(ambientLoc, 1, glm::value_ptr(AMBIENT));
                    glUniform1f(alphaLoc, alpha[i]);
                }
            
                if (BEAM_ID != i  &&  1.0f != alpha[i])
                    glEnable(GL_DEPTH_TEST);
            }

            updateState(STEP_SECONDS, meshes);

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
