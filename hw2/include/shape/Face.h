#ifndef FACE_H
#define FACE_H

#include <memory>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shape/GLShape.h"
#include "shape/Circle.h"
#include "shape/Triangle.h"
#include "shape/Ball.h"

class Shader;

// class Face : public Renderable, public GLShape
class Face : public Renderable, public GLShape
{
public:
    Face(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity);

    ~Face() noexcept override = default;

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    bool checkCollisionOthers();
    bool checkCollisionBoundary();

    glm::vec2 getPosition(){
        return position;
    }

    float getRadiusNDC(){
        return radiusNDC;
    }

    private:
    glm::vec3 parameters;
    
    glm::vec2 originNDC;
    float radiusNDC;
    glm::vec2 position;
    glm::vec2 velocity;

    void PhysicsUpdate(float dt);
};

#endif
