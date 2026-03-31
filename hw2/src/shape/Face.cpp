#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "shape/Ball.h"
#include "app/App.h"
#include "shape/Circle.h"
#include "shape/Face.h"
#include "util/Shader.h"

Face::Face(Shader * circleShader, Shader * triangleShader, const glm::vec3 parameters, glm::vec2 velocity)
    : Ball(circleShader, parameters, velocity), pTriangleShader(triangleShader)
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

    // initialize circle list for this face
    double radius = parameters.z;

    circ3s.emplace_back(Circ3{parameters});
    //left eye
    circ3s.emplace_back(Circ3{{parameters.x - 0.5 * radius,
                              parameters.y,
                              parameters.z/2.0f}});
    //right eye
    circ3s.emplace_back(Circ3{{parameters.x + 0.5 * radius,
                              parameters.y,
                              parameters.z/2.0f}});

    triVerts.emplace_back(Vertex{{parameters.x - 0.25f * radius, parameters.y - 0.5f * radius}, {1.0f, 0.0f, 0.0f}});
    triVerts.emplace_back(Vertex{{parameters.x, parameters.y - 0.75f * radius}, {0.0f, 1.0f, 0.0f}});
    triVerts.emplace_back(Vertex{{parameters.x + 0.25f * radius, parameters.y - 0.5f * radius}, {0.0f, 0.0f, 1.0f}});
    


    //-----------------CIRCLE DRAWING BOILER PLATE-----------------
    glGenVertexArrays(1, &vaoC);
    glGenBuffers(1, &vboC);
    glBindVertexArray(vaoC);
    glBindBuffer(GL_ARRAY_BUFFER, vboC);

    // Vertex coordinate attribute array "layout (position = 0) in vec3 aPos"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                             // index: corresponds to "0" in "layout (position = 0)"
                          3,                             // size: each "vec3" generic vertex attribute has 3 values
                          GL_FLOAT,                      // data type: "vec3" generic vertex attributes are GL_FLOAT
                          GL_FALSE,                      // do not normalize data
                          sizeof(Circ3),             // stride between attributes in VBO data
                          reinterpret_cast<void *>(0));  // offset of 1st attribute in VBO data

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(circ3s.size() * sizeof(Circ3)),
                 circ3s.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //-----------------TRIANGLE DRAWING BOILERPLATE-----------------
    glGenVertexArrays(1, &vaoT);
    glGenBuffers(1, &vboT);
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
                 static_cast<GLsizei>(triVerts.size() * sizeof(Vertex)),
                 triVerts.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
bool Face::checkCollisionOthers(){
    
    for (auto& shape : App::getInstance().getShapes()){
        Face * other = dynamic_cast<Face*>(shape.get());

        if (other == this || !other){
            continue;
        }
        // glm::vec2 posVP = glm::inverse(ortho) * glm::vec3(position.x, position.y, 1);
        glm::vec2 diff = other->getPosition() - position;
        float dist = glm::length(diff);
        if (dist <= other->getRadiusNDC() + radiusNDC){
            //handle the collision
            glm::vec2 n = diff / dist; //normalize the vector
            glm::vec2 v1n = glm::dot(velocity, n) * n;
            glm::vec2 v1t = velocity - v1n;

            glm::vec2 v2n = glm::dot(other->velocity, n) * n;
            glm::vec2 v2t = other->velocity - v2n;

            velocity = v2n + v1t;
            other->velocity = v1n + v2t;

            //correct positions
            float penetration = radiusNDC + other->getRadiusNDC() - dist;
            glm::vec2 correction = 0.5f * penetration * n;
            position -= correction;
            other->position += correction;
            
            other->markEvolve();
            markEvolve();
            return true;
        }
    }
    return false;
}

// CelestialBody::CelestialBody()
void Face::PhysicsUpdate(float dt){
    // velocity += gravNDC * dt;
    velocity += gravNDC;
    position += velocity * dt;    
    model = glm::translate(glm::mat3(1.0f), position - originNDC);
    
    if (Ball::checkCollisionBoundary()){
        float r = 2.0f * parameters.z / App::getWindowWidth();
        position = glm::clamp(position, glm::vec2(-1+radiusNDC,-1+radiusNDC), glm::vec2(1-radiusNDC,1-radiusNDC));
    } 

    if (Face::checkCollisionOthers()){
        std::cout << "COLLISION WITH OTHER BALLS" << std::endl;
        // evolve();
    }

    if (evolveFlag){
        evolve();
        evolveFlag = false;
    }
}

void Face::evolve(){
    drawFace();
}

void Face::drawFace(){
    std::cout << "attempt to draw face:" << std::endl;

    int n = generation;
    int level = (int)log2(n + 1);
    int denom = 1 << level; // 2^level

    // The numerator is the "remainder" mapped to odd integers
    int numerator = 2 * (n - (denom - 1)) - (denom - 1);
    

    double x = -(double)numerator / denom; // the xposition for the eye

    double radius = parameters.z;

    //original face coordinates
    glm::vec3 leftEye(parameters.x - 0.5 * radius,parameters.y, 1);
    glm::vec3 rightEye(parameters.x + 0.5 * radius,parameters.y, 1);
    glm::vec3 mouth_a(parameters.x - 0.25f * radius, parameters.y - 0.5f * radius,1);
    glm::vec3 mouth_b(parameters.x, parameters.y - 0.75f * radius, 1);
    glm::vec3 mouth_c(parameters.x + 0.25f * radius, parameters.y - 0.5f * radius,1);

    glm::mat3 T1 = glm::translate(glm::mat3(1.0f), -glm::vec2(parameters.x,parameters.y));
    glm::mat3 S1 = glm::scale(glm::mat3(1.0f), -glm::vec2(1/radius,1/radius));
    glm::mat3 normalize = S1 * T1;

    glm::mat3 T2 = glm::translate(glm::mat3(1.0f), glm::vec2(x,0));
    glm::mat3 S2 = glm::scale(glm::mat3(1.0f), glm::vec2(1.0f/denom,1.0f/denom));

    glm::mat3 final = glm::inverse(normalize) * T2 * S2 * normalize;

    leftEye = final * leftEye;
    rightEye = final * rightEye;
    mouth_a = final * mouth_a;
    mouth_b = final * mouth_b;
    mouth_c = final * mouth_c;

    double newRadius = radius / (2 * denom);
    leftEye.z = newRadius;
    rightEye.z = newRadius;

    circ3s.emplace_back(Circ3{leftEye});
    circ3s.emplace_back(Circ3{rightEye});
    triVerts.emplace_back(Vertex{{glm::vec2(mouth_a)}, {1.0f, 0.0f, 0.0f}});
    triVerts.emplace_back(Vertex{{glm::vec2(mouth_b)}, {0.0f, 1.0f, 0.0f}});
    triVerts.emplace_back(Vertex{{glm::vec2(mouth_c)}, {0.0f, 0.0f, 1.0f}});

    generation++;

    // Upload updated circle data to GPU so new circles are actually rendered.
    glBindVertexArray(vaoC);
    glBindBuffer(GL_ARRAY_BUFFER, vboC);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(circ3s.size() * sizeof(Circ3)),
                 circ3s.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // same idea but with triangles
    glBindVertexArray(vaoT);
    glBindBuffer(GL_ARRAY_BUFFER, vboT);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(triVerts.size() * sizeof(Vertex)),
                 triVerts.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// void Face::drawFace(){
//     generation++;
//     circ3s.clear();
//     triVerts.clear();
//     int depth = (int)floor(log2(generation+1));
//     // drawFaceGen(depth, parameters.x, parameters.y, parameters.z);
//     glBindVertexArray(vaoC);
//     glBindBuffer(GL_ARRAY_BUFFER, vboC);
//     glBufferData(GL_ARRAY_BUFFER,
//                  static_cast<GLsizei>(circ3s.size() * sizeof(Circ3)),
//                  circ3s.data(),
//                  GL_DYNAMIC_DRAW);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);

//     // same idea but with triangles
//     glBindVertexArray(vaoT);
//     glBindBuffer(GL_ARRAY_BUFFER, vboT);
//     glBufferData(GL_ARRAY_BUFFER,
//                  static_cast<GLsizei>(triVerts.size() * sizeof(Vertex)),
//                  triVerts.data(),
//                  GL_DYNAMIC_DRAW);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);
// }

// void Face::drawFaceGen(int generation, float cx, float cy, float radius) {
//     if (generation == 0) {
//         // just draw a plain eye circle
//         circ3s.emplace_back(Circ3{{cx, cy, radius}});
//         return;
//     }

//     float eyeRadius = radius / 2.0f;

//     // face circle
//     circ3s.emplace_back(Circ3{{cx, cy, radius}});

//     // left eye - recurse if generation warrants it
//     float lx = cx - 0.5f * radius;
//     float ly = cy;
//     drawFaceGen(generation - 1, lx, ly, eyeRadius);

//     // right eye
//     float rx = cx + 0.5f * radius;
//     float ry = cy;
//     drawFaceGen(generation - 1, rx, ry, eyeRadius);

//     // mouth triangle
//     triVerts.emplace_back(Vertex{{cx - 0.25f*radius, cy - 0.5f*radius}, {1,0,0}});
//     triVerts.emplace_back(Vertex{{cx,                cy - 0.75f*radius}, {0,1,0}});
//     triVerts.emplace_back(Vertex{{cx + 0.25f*radius, cy - 0.5f*radius}, {0,0,1}});
// }

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
                 static_cast<GLsizei>(circ3s.size()));  // draw these number of elements

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // draw the triangles as outline (no fill)
    if (pTriangleShader)
    {
        pTriangleShader->use();
        pTriangleShader->setMat3("model", model);
        pTriangleShader->setFloat("windowWidth", App::getWindowWidth());
        pTriangleShader->setFloat("windowHeight", App::getWindowHeight());

        glBindVertexArray(vaoT);
        glBindBuffer(GL_ARRAY_BUFFER, vboT);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES,
                0,
                static_cast<GLsizei>(triVerts.size()));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

}