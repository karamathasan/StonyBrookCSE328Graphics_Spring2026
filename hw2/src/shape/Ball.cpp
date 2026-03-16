#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "shape/Ball.h"
#include "app/App.h"
#include "shape/Circle.h"
#include "util/Shader.h"

// glm::mat3 ortho = {
//     {2.0f/1000, 0, -1.0f},
//     {0, 2.0f/1000, -1.0f},
//     {0, 0, 1.0f}
// };


const glm::mat3 ortho = {
    {2.0f/App::getWindowWidth(), 0, 0},
    {0, 2.0f/App::getWindowHeight(), 0},
    {-1.0f, -1.0f, 1.0f}
};

glm::vec3 gravHom(0.0f,-9.8f,0.0f); //prevents shifts
glm::vec2 gravNDC = (ortho * gravHom);

Ball::Ball(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity) : GLShape(shader), parameters(parameters), velocity(velocity)
{
    glm::vec3 originHomog(parameters.x, parameters.y, 1);
    originNDC = ortho * originHomog;
    position = originNDC;

    radiusNDC = 2.0f * parameters.z / App::getWindowWidth();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertex coordinate attribute array "layout (position = 0) in vec3 aPos"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                             // index: corresponds to "0" in "layout (position = 0)"
                          3,                             // size: each "vec3" generic vertex attribute has 3 values
                          GL_FLOAT,                      // data type: "vec3" generic vertex attributes are GL_FLOAT
                          GL_FALSE,                      // do not normalize data
                          sizeof(glm::vec3),             // stride between attributes in VBO data
                          reinterpret_cast<void *>(0));  // offset of 1st attribute in VBO data

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(sizeof(glm::vec3)),
                 &parameters,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool Ball::checkCollisionOthers(std::vector<Ball> balls){
    for (const Ball & other : balls){
        
    }
    return false;
}

bool Ball::checkCollisionBoundary(){

    float x = position.x;
    float y = position.y;
    
    bool xBounded = -1 + radiusNDC <= x && x <= 1 - radiusNDC;
    bool yBounded = -1 + radiusNDC <= y && y <= 1 - radiusNDC;

    if (!xBounded){
        velocity.x = -velocity.x;
    }
    if (!yBounded){
        velocity.y = -velocity.y;
    }
    return !(xBounded && yBounded);
}

void Ball::PhysicsUpdate(float dt){
    if (Ball::checkCollisionBoundary()){
        float r = 2.0f * parameters.z / App::getWindowWidth();
        position = glm::clamp(position, glm::vec2(-1+radiusNDC,-1+radiusNDC), glm::vec2(1-radiusNDC,1-radiusNDC));
    } 
    
    // velocity += gravNDC * dt;
    velocity += gravNDC;
    position += velocity * dt;
    // std::cout << "position: " << position.x <<", "<< position.y << std::endl;
    
    model = glm::translate(glm::mat3(1.0f), position - originNDC);
}

void Ball::render(float timeElapsedSinceLastFrame, bool animate)
{
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
