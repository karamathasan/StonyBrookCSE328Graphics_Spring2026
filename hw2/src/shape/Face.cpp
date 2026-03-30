#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "shape/Ball.h"
#include "app/App.h"
#include "shape/Circle.h"
#include "shape/Face.h"
#include "util/Shader.h"

Face::Face(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity): Ball(shader, parameters, velocity)
{
    glm::mat3 ortho = {
        {2.0f/App::getWindowWidth(), 0, 0},
        {0, 2.0f/App::getWindowHeight(), 0},
        {-1.0f, -1.0f, 1.0f}
    };

    glm::vec3 originHomog(parameters.x, parameters.y, 1);
    originNDC = ortho * originHomog;
    position = originNDC;

    radiusNDC = 2.0f * parameters.z / App::getWindowWidth();

    //CIRCLE DRAWING BOILER PLATE
    glBindVertexArray(vaoC);
    glBindBuffer(GL_ARRAY_BUFFER, vboC);

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

    //TRIANGLE DRAWING BOILERPLATE
    glBindVertexArray(vaoT);
    glBindBuffer(GL_ARRAY_BUFFER, vboT);

    // Vertex coordinate attribute array "layout (position = 0) in vec2 aPosition"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                             // index: corresponds to "0" in "layout (position = 0)"
                          2,                             // size: each "vec2" generic vertex attribute has 2 values
                          GL_FLOAT,                      // data type: "vec2" generic vertex attributes are GL_FLOAT
                          GL_FALSE,                      // do not normalize data
                          sizeof(Vertex),                // stride between attributes in VBO data
                          reinterpret_cast<void *>(0));  // offset of 1st attribute in VBO data

    // Color vertex attribute array "layout (position = 1) in vec3 aColor"
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          reinterpret_cast<void *>(sizeof(Vertex::position)));

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(verticesNDC.size() * sizeof(Vertex)),
                 verticesNDC.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

// CelestialBody::CelestialBody()
void Face::PhysicsUpdate(float dt){
    velocity += gravNDC * dt;
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


void Face::render(float timeElapsedSinceLastFrame, bool animate){
    if (animate)
    {
        PhysicsUpdate(timeElapsedSinceLastFrame);
    }

    pShader->use();
    pShader->setMat3("model", model);

    glBindVertexArray(vaoC);
    glBindBuffer(GL_ARRAY_BUFFER, vboC);

    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays(GL_PATCHES,
                 0,                                          // start from index 0 in current VBO
                 static_cast<GLsizei>(3));  // draw these number of elements

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(vaoT);
    glBindBuffer(GL_ARRAY_BUFFER, vboT);

    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays(GL_PATCHES,
                 0,                                          // start from index 0 in current VBO
                 static_cast<GLsizei>(3));  // draw these number of elements

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}