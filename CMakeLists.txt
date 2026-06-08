cmake_minimum_required(VERSION 3.16)

project(UFO_Sheep_Abduction LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(UFO_Sheep_Abduction
    main.cpp
    Model/OBJFile.cpp
    Model/ColoredMesh.cpp
)

target_include_directories(UFO_Sheep_Abduction PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Model
    ${CMAKE_CURRENT_SOURCE_DIR}/Shader
    ${CMAKE_CURRENT_SOURCE_DIR}/glm
)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(UFO_Sheep_Abduction PRIVATE OpenGL::GL glfw GLEW::GLEW)

if (WIN32)
    add_custom_command(TARGET UFO_Sheep_Abduction POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/glew32.dll
            $<TARGET_FILE_DIR:UFO_Sheep_Abduction>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/glfw3.dll
            $<TARGET_FILE_DIR:UFO_Sheep_Abduction>
    )
endif()