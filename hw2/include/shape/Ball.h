#ifndef BALL_H
#define BALL_H

#include <memory>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shape/GLShape.h"
#include "shape/Circle.h"

class Shader;

// const glm::mat3 ortho = {
//     {2.0f/App::getWindowWidth(), 0, 0},
//     {0, 2.0f/App::getWindowHeight(), 0},
//     {-1.0f, -1.0f, 1.0f}
// };

// glm::vec3 gravHom(0.0f,-9.8f,0.0f); //prevents shifts
// glm::vec2 gravNDC = (ortho * gravHom);

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
    
    glm::vec2 getPosition(){
        return position;
    }
    
    void setVelocity(glm::vec2 velocity){
        this->velocity = velocity;
    }

    float getRadiusNDC(){
        return radiusNDC;
    }


protected:
// private:
    // std::vector<glm::vec3> parameters;
    glm::vec3 parameters;
    
    glm::vec2 originNDC;
    float radiusNDC;
    glm::vec2 position;
    glm::vec2 velocity;

    // static glm::mat3 ortho;
    static glm::vec2 gravNDC;

    void PhysicsUpdate(float dt);
};



#endif
