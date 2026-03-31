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
        Vertex() = default;
        Vertex(const glm::vec2 & p, const glm::vec3 & c) : position(p), color(c) {}
    };

    struct Circ3 //circle with vec 3, bad name, i know
    {
        glm::vec3 params;
        Circ3() = default;
        Circ3(const glm::vec3 & _params) : params(_params) {}
        Circ3(float x, float y, float r) : params(x,y,r) {}
    };

    Face(Shader * circleShader, Shader * triangleShader, const glm::vec3 parameters, glm::vec2 velocity);

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    void evolve();
    void drawFace();
    // void drawFaceGen(int generation, float cx, float cy, float radius);


    GLuint vaoC {0U};
    GLuint vboC {0U};

    GLuint vaoT {0U};
    GLuint vboT {0U};

    Shader * pTriangleShader {nullptr};

    void markEvolve(){
        // std::cout << "marked for evolution" << std::endl;
        evolveFlag = true;
    }

private:
    std::vector<Circ3> circ3s; //global reference frame
    std::vector<Vertex> triVerts; // global reference frame
    int generation = 1;

    bool evolveFlag = false;

    void PhysicsUpdate(float dt);
    bool checkCollisionOthers();
    // bool 
};



#endif
