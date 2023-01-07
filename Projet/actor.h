#ifndef ACTOR_H
#define ACTOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "model.h"
#include "mesh.h"
class Actor
{
public:

    Actor(void* obj, bool isShape = false, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rot = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 sc = glm::vec3(1.0f, 1.0f, 1.0f)) :
        position(pos), rotationAxis(rot), scale(sc), isShape(isShape) {
        objToDraw = obj;
       
	}
    Actor() {

    }


    void draw(Shader shader) {
        shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model,position);
        model = glm::translate(model, Offset);
        model = glm::rotate(model,glm::radians(rotationAngle), rotationAxis);
        model = glm::scale(model, scale);
        shader.setMat4("model", model);
        if (isShape) {
            ((cube*)objToDraw)->draw();
            return;
        }
        ((Model*)objToDraw)->Draw(shader);

    }
    glm::vec3 position;
    glm::vec3 rotationAxis = glm::vec3(1, 1, 1);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    glm::vec3 Offset = glm::vec3(0, 0, 0);
    float rotationAngle = 0;
    bool shouldRotate = false;
    bool shouldMove = false;
    bool isShape;
protected:
    void* objToDraw;
};


#endif 

