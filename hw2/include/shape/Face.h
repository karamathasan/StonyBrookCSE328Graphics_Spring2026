#ifndef FACE_H
#define FACE_H

#include <memory>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shape/GLShape.h"
#include "shape/Circle.h"
#include "shape/Ball.h"

class Shader;

// class Face : public Ball, public GLShape, public Renderable
class Face : public Ball
{
public:
    using Ball::Ball;
    struct Vertex
    {
        glm::vec2 position;
        glm::vec3 color;
    };
    struct CircleParam
    {
        glm::vec2 position;
        float radius;
    };

    Face(Shader * shader, const glm::vec3 parameters, glm::vec2 velocity);

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    GLuint vaoC {0U};
    GLuint vboC {0U};

    GLuint vaoT {0U};
    GLuint vboT {0U};

private:
    std::vector<Vertex> verticesNDC; // global reference frame
    int generation = 1;

    void PhysicsUpdate(float dt);
};



#endif
