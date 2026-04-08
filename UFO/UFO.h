#ifndef UFO_H
#define UFO_H

#include "../Model/Model.h" 

class UFO : public Model {
public:
    glm::vec3 position;
    
    UFO() {
        position = glm::vec3(0.0f, 5.0f, 0.0f); 
    }

    void move(glm::vec3 offset) {
        position += offset; // Klavye girdisiyle hareket [cite: 55]
        modelMatrix = glm::translate(glm::mat4(1.0f), position);
    }

    void draw(GLuint program, glm::mat4 projection, glm::mat4 view) override;
};

#endif