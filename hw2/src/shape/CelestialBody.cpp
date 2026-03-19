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
        float mass = 1.0f;
        float G = 0.01f;
        glm::vec2 f = dir * G * mass * mass / r2;
        net += f;
    }
    return net;
}

// CelestialBody::CelestialBody()
void CelestialBody::PhysicsUpdate(float dt){

    velocity += findForce() * dt;
    position += velocity * dt;    
    model = glm::translate(glm::mat3(1.0f), position - originNDC);
    
    if (Ball::checkCollisionBoundary()){
        float r = 2.0f * parameters.z / App::getWindowWidth();
        position = glm::clamp(position, glm::vec2(-1+radiusNDC,-1+radiusNDC), glm::vec2(1-radiusNDC,1-radiusNDC));
    } 

    if (Ball::checkCollisionOthers()){
        // std::cout << "COLLISION WITH OTHER BALLS" << std::endl;
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