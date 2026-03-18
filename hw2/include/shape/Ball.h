#ifndef BALL_H
#define BALL_H

#include <memory>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shape/GLShape.h"
#include "shape/Circle.h"

class Shader;

class Ball : public Renderable, public GLShape
{
public:
    Ball(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity);
    // Ball(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity, std::vector<std::unique_ptr<Ball>>& envBalls);

    ~Ball() noexcept override = default;

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    // bool checkCollisionOthers(std::vector<Ball>);
    bool checkCollisionOthers();
    bool checkCollisionBoundary();
    
    // float radius;

    glm::vec2 getPosition(){
        return position;
    }

    float getRadiusNDC(){
        return radiusNDC;
    }

private:
    // std::vector<glm::vec3> parameters;
    glm::vec3 parameters;
    
    glm::vec2 originNDC;
    float radiusNDC;
    glm::vec2 position;
    glm::vec2 velocity;

    // std::vector<Ball*>& envBalls;
    // std::vector<std::unique_ptr<Ball>>& envBalls;

    void PhysicsUpdate(float dt);
};



#endif
