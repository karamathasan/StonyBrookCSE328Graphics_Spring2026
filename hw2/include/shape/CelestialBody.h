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
    struct ParseParams{
        float mass;
        float radius;
        glm::vec2 position;
        glm::vec2 velocity;
        // ParseParams(float _mass, float _radius, glm::vec2 _position, glm::vec2 _velocity) : mass(_mass),  
    };
    using Ball::Ball;
    // CelestialBody(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity, float mass);
    CelestialBody(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity, float mass) : Ball(shader, parameters, velocity), mass(mass) {}

    // ~CelestialBody() noexcept override = default;

    void render(float timeElapsedSinceLastFrame, bool animate) override;
    bool checkCollisionOthers() override;

    glm::vec2 findForce();
    static constexpr float G = 0.05f;
    
    float mass = 1.0f;

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
