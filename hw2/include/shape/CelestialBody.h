#ifndef CELESTIALBODY_H
#define CELESTIALBODY_H

#include <memory>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shape/GLShape.h"
#include "shape/Circle.h"
#include "shape/Ball.h"

class Shader;

class CelestialBody : public Ball
{
public:
    using Ball::Ball;
    // CelestialBody(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity);

    // ~CelestialBody() noexcept override = default;

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    glm::vec2 findForce();
    // bool checkCollisionOthers();
    // bool checkCollisionBoundary();
    

    // glm::vec2 getPosition(){
    //     return position;
    // }

    // float getRadiusNDC(){
    //     return radiusNDC;
    // }

private:
    // glm::vec3 parameters;
    
    // glm::vec2 originNDC;
    // float radiusNDC;
    // glm::vec2 position;
    // glm::vec2 velocity;

    void PhysicsUpdate(float dt);
};



#endif
