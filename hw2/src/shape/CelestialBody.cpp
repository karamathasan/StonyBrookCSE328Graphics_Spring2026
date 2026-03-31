#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "shape/Ball.h"
#include "app/App.h"
#include "shape/Circle.h"
#include "util/Shader.h"
#include "shape/CelestialBody.h"


glm::vec2 CelestialBody::findForce(){
    glm::vec2 net(0.0f,0.0f);
    for (auto& shape : App::getInstance().getShapes()){
        CelestialBody* other = dynamic_cast<CelestialBody*>(shape.get());
        if (!other || other == this){
            continue;
        }

        glm::vec2 diff = other->getPosition() - position;
        glm::vec2 dir = glm::normalize(diff);
        float r2 = glm::dot(diff,diff);

        //masses are both assumed to be 1
        // float mass = 1.0f;
        glm::vec2 f = dir * G * mass * other->mass / r2;
        net += f;
    }
    return net;
}

// CelestialBody::CelestialBody()
bool CelestialBody::checkCollisionOthers(){

    for (auto& shape : App::getInstance().getShapes()){
        CelestialBody* other = dynamic_cast<CelestialBody*>(shape.get());

        if (!other || other == this){
            continue;
        }

        glm::vec2 diff = other->getPosition() - position;
        float dist = glm::length(diff);
        float sumR = other->getRadiusNDC() + radiusNDC;

        if (dist <= sumR && dist > 1e-5f){
            glm::vec2 n = diff / dist;

            // 1D elastic collision along normal, with mass.
            float m1 = mass;
            float m2 = other->mass;

            float v1n = glm::dot(velocity, n);
            float v2n = glm::dot(other->velocity, n);

            float v1n_new = (v1n * (m1 - m2) + 2.0f * m2 * v2n) / (m1 + m2);
            float v2n_new = (v2n * (m2 - m1) + 2.0f * m1 * v1n) / (m1 + m2);

            glm::vec2 v1t = velocity - v1n * n;
            glm::vec2 v2t = other->velocity - v2n * n;

            velocity = v1t + v1n_new * n;
            other->velocity = v2t + v2n_new * n;

            float penetration = sumR - dist;
            glm::vec2 correction = 0.5f * penetration * n;
            position -= correction;
            other->position += correction;

            return true;
        }
    }

    return false;
}

// CelestialBody::CelestialBody()
void CelestialBody::PhysicsUpdate(float dt){

    velocity += findForce()/mass * dt;
    position += velocity * dt;    
    model = glm::translate(glm::mat3(1.0f), position - originNDC);
    
    if (Ball::checkCollisionBoundary()){
        float r = 2.0f * parameters.z / App::getWindowWidth();
        position = glm::clamp(position, glm::vec2(-1+radiusNDC,-1+radiusNDC), glm::vec2(1-radiusNDC,1-radiusNDC));
    } 

    if (CelestialBody::checkCollisionOthers()){
        // std::cout << "COLLISION WITH OTHER BODIES" << std::endl;
    }
}


void CelestialBody::render(float timeElapsedSinceLastFrame, bool animate){
    if (animate)
    {
        PhysicsUpdate(timeElapsedSinceLastFrame);
    }

    pShader->use();
    pShader->setMat3("model", model);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays(GL_PATCHES,
                 0,                                          // start from index 0 in current VBO
                 static_cast<GLsizei>(3));  // draw these number of elements

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}