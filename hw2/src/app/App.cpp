#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/App.h"
#include "shape/Circle.h"
#include "shape/Triangle.h"
#include "shape/Ball.h"
#include "shape/Face.h"
#include "shape/CelestialBody.h"
#include "util/Shader.h"

#include <iostream>
#include <string>
std::string state = "balls";

#include <fstream>
#include <sstream>

App & App::getInstance()
{
    static App instance;
    return instance;
}

void App::run()
{
    while (!glfwWindowShouldClose(pWindow))
    {
        // Per-frame logic
        perFrameTimeLogic(pWindow);
        processKeyInput(pWindow);

        // Send render commands to OpenGL server
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        render();

        // Check and call events and swap the buffers
        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }
}

glm::vec3 App::parse(){

    std::string fp = "etc/config.txt";
    std::ifstream file(fp);

    if (!file.is_open()){
        std::cerr << "error opening file" << std::endl;
        return glm::vec3(0.001,0,0);
    }

    std::string line;
    if (!std::getline(file, line)){
        std::cerr << "file is empty" << std::endl;   
    }
    
    file.close();

    std::stringstream ss(line);
    std::string r;
    std::string vx;
    std::string vy;
    std::getline(ss, r, ',');    
    std::getline(ss, vx, ',');    
    std::getline(ss, vy, ',');    

    return glm::vec3(std::stof(vx), std::stof(vy), std::stof(r));
}

std::vector<CelestialBody::ParseParams> App::parse_celestial(){

    std::string fp = "etc/celest_config.txt";
    std::ifstream file(fp);

    std::vector<CelestialBody::ParseParams> out;

    if (!file.is_open()){
        std::cerr << "error opening file" << std::endl;
        return out;
    }

    std::string line;

    while (std::getline(file, line)){
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string m;
        std::string r;
        std::string x;
        std::string y;
        std::string vx;
        std::string vy;
        std::getline(ss, m, ',');    
        std::getline(ss, r, ',');    
        std::getline(ss, x, ',');    
        std::getline(ss, y, ','); 
        std::getline(ss, vx, ',');    
        std::getline(ss, vy, ','); 

        CelestialBody::ParseParams params;
        params.mass = std::stof(m);
        params.position = glm::vec2(std::stof(x), std::stof(y));
        params.radius = std::stof(r);
        params.velocity = glm::vec2(std::stof(vx), std::stof(vy));
        out.push_back(params);
    }

    return out;
}

void App::cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    app.mousePos.x = xpos;
    app.mousePos.y = App::kWindowHeight - ypos;

    if (app.mousePressed)
    {
        // // Note: Must calculate offset first, then update lastMouseLeftPressPos.
        // glm::dvec2 offset = app.mousePos - app.lastMouseLeftPressPos;
        app.lastMouseLeftPressPos = app.mousePos;
    }
}


void App::framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void App::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        app.animationEnabled = !app.animationEnabled;
    }

    if (key == GLFW_KEY_1 && action == GLFW_RELEASE && state != "balls")
    {
        app.clearShapes();
        state = "balls";
        return;
    }

    if (key == GLFW_KEY_3 && action == GLFW_RELEASE && state != "faces")
    {
        app.clearShapes();
        state = "faces";
        return;
    }

    if (key == GLFW_KEY_4 && action == GLFW_RELEASE && state != "celestial")
    {
        app.clearShapes();
        state = "celestial";
        for (CelestialBody::ParseParams params : parse_celestial()){
            float mass = params.mass;
            glm::vec2 positionGlob(
                (params.position.x + 1) * App::getWindowWidth()/2 ,
                (params.position.y + 1) * App::getWindowHeight()/2
            );
            glm::vec3 circ(positionGlob, params.radius * App::getWindowWidth()/2);
            // glm::vec2 vel(param.vx, params.vy);


            app.shapes.emplace_back(
                std::make_unique<CelestialBody>(
                    app.pCircleShader.get(),
                    circ,
                    params.velocity,
                    mass
                )
            ); 
        }
        return;
    }
       
}


void App::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            app.mousePressed = true;
            app.lastMouseLeftClickPos = app.mousePos;
            app.lastMouseLeftPressPos = app.mousePos;

            // spawning logic
            if (state == "balls"){
                app.spawnBall(app);
            } 
            else if (state == "faces"){
                app.spawnFace(app);
            }
            else if (state == "celestial"){
                app.spawnCelestialBody(app);
            }
        }
        else if (action == GLFW_RELEASE)
        {
            app.mousePressed = false;

            #ifdef DEBUG_MOUSE_POS
            std::cout << "[ " << app.mousePos.x << ' ' << app.mousePos.y << " ]\n";
            #endif
        }
    }
}

void App::clearShapes(){
    shapes.clear();
}

void App::spawnBall(App & app){
    glm::vec3 init = App::parse();
    float vx = init.x;
    float vy = init.y;
    // float r = (init.z) * 500.0f; //convert to viewport float coords
    float r = (init.z) * App::getWindowWidth()/2.0f; //convert to viewport float coords
    glm::vec3 params(app.lastMouseLeftClickPos.x, app.lastMouseLeftClickPos.y, r);
        
    //check if spawn would cause a collision
    //with boundary

    glm::vec2 positionNDC(
        2 * app.lastMouseLeftClickPos.x / App::getWindowWidth() - 1.0,
        2 * app.lastMouseLeftClickPos.y / App::getWindowHeight() - 1.0
    );
    bool xBounded = -1 + init.z <= positionNDC.x && positionNDC.x <= 1 - init.z;
    bool yBounded = -1 + init.z <= positionNDC.y && positionNDC.y <= 1 - init.z;

    if (!(xBounded && yBounded)){
        std::cout << "rejection by bounds" << std::endl;
        return;
    }

    //with other balls
    for (auto& shape : App::getInstance().getShapes()){
        Ball * other = dynamic_cast<Ball*>(shape.get());

        if (!other){
            continue;
        }

        glm::vec2 diff = other->getPosition() - positionNDC;
        float dist = glm::length(diff);
        if (dist <= other->getRadiusNDC() + init.z){
            std::cout << "rejection by other ball" << std::endl;
            return;
        }
    }
    
    shapes.emplace_back(
        std::make_unique<Ball>(
            pCircleShader.get(),
            params,
            glm::vec2(vx,vy)
        )
    );   

}

void App::spawnFace(App & app){
    glm::vec3 init = App::parse();
    float vx = init.x;
    float vy = init.y;
    float r = (init.z) * 500.0f; //convert to viewport float coords
    glm::vec3 params(app.lastMouseLeftClickPos.x, app.lastMouseLeftClickPos.y, r);
        
    //check if spawn would cause a collision
    //with boundary

    glm::vec2 positionNDC(
        2 * app.lastMouseLeftClickPos.x / App::getWindowWidth() - 1.0,
        2 * app.lastMouseLeftClickPos.y / App::getWindowHeight() - 1.0
    );
    bool xBounded = -1 + init.z <= positionNDC.x && positionNDC.x <= 1 - init.z;
    bool yBounded = -1 + init.z <= positionNDC.y && positionNDC.y <= 1 - init.z;

    if (!(xBounded && yBounded)){
        std::cout << "rejection by bounds" << std::endl;
        return;
    }

    //with other balls
    for (auto& shape : App::getInstance().getShapes()){
        Ball * other = dynamic_cast<Ball*>(shape.get());

        if (!other){
            continue;
        }

        glm::vec2 diff = other->getPosition() - positionNDC;
        float dist = glm::length(diff);
        if (dist <= other->getRadiusNDC() + init.z){
            std::cout << "rejection by other ball" << std::endl;
            return;
        }
    }
    
    shapes.emplace_back(
        std::make_unique<Face>(
            pCircleShader.get(),
            pTriangleShader.get(),
            params,
            glm::vec2(vx,vy)
        )
    );
}

void App::spawnCelestialBody(App & app){
    glm::vec3 init = App::parse();
    float vx = init.x;
    float vy = init.y;
    float r = (init.z) * 500.0f; //convert to viewport float coords
    glm::vec3 params(app.lastMouseLeftClickPos.x, app.lastMouseLeftClickPos.y, r);
        
    //check if spawn would cause a collision
    //with boundary
    std::cout << "attempting to spawn celestial body" << std::endl;

    glm::vec2 positionNDC(
        2 * app.lastMouseLeftClickPos.x / App::getWindowWidth() - 1.0,
        2 * app.lastMouseLeftClickPos.y / App::getWindowHeight() - 1.0
    );
    bool xBounded = -1 + init.z <= positionNDC.x && positionNDC.x <= 1 - init.z;
    bool yBounded = -1 + init.z <= positionNDC.y && positionNDC.y <= 1 - init.z;

    if (!(xBounded && yBounded)){
        std::cout << "rejection by bounds" << std::endl;
        return;
    }

    //with other celestial bodies
    for (auto& shape : App::getInstance().getShapes()){
        CelestialBody * other = dynamic_cast<CelestialBody*>(shape.get());

        if (!other){
            continue;
        }

        glm::vec2 diff = other->getPosition() - positionNDC;
        float dist = glm::length(diff);
        if (dist <= other->getRadiusNDC() + init.z){
            std::cout << "rejection by other ball" << std::endl;
            return;
        }
    }
    
    //mass is default to 1
    shapes.emplace_back(
        std::make_unique<CelestialBody>(
            pCircleShader.get(),
            params,
            glm::vec2(vx,vy)
        )
    );   
}



void App::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{

}


void App::perFrameTimeLogic(GLFWwindow * window)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    double currentFrame = glfwGetTime();
    app.timeElapsedSinceLastFrame = currentFrame - app.lastFrameTimeStamp;
    app.lastFrameTimeStamp = currentFrame;
}


void App::processKeyInput(GLFWwindow * window)
{

}


App::App() : Window(kWindowWidth, kWindowHeight, kWindowName, nullptr, nullptr)
{
    // GLFW boilerplate.
    glfwSetWindowUserPointer(pWindow, this);
    glfwSetCursorPosCallback(pWindow, cursorPosCallback);
    glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);
    glfwSetKeyCallback(pWindow, keyCallback);
    glfwSetMouseButtonCallback(pWindow, mouseButtonCallback);
    glfwSetScrollCallback(pWindow, scrollCallback);

    // Global OpenGL pipeline settings
    glViewport(0, 0, kWindowWidth, kWindowHeight);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
    glPointSize(1.0f);

    // Initialize shaders and objects-to-render;
    pTriangleShader = std::make_unique<Shader>("src/shader/triangle.vert.glsl",
                                               "src/shader/triangle.frag.glsl");
    pCircleShader = std::make_unique<Shader>("src/shader/circle.vert.glsl",
                                             "src/shader/circle.tesc.glsl",
                                             "src/shader/circle.tese.glsl",
                                             "src/shader/circle.frag.glsl");

    // shapes.emplace_back(
    //         std::make_unique<Triangle>(
    //                 pTriangleShader.get(),
    //                 std::vector<Triangle::Vertex> {
    //                         // Vertex coordinate (screen-space coordinate), Vertex color
    //                         {{200.0f, 326.8f}, {1.0f, 0.0f, 0.0f}},
    //                         {{800.0f, 326.8f}, {0.0f, 1.0f, 0.0f}},
    //                         {{500.0f, 846.4f}, {0.0f, 0.0f, 1.0f}},
    //                 }
    //         )
    // );


    // shapes.emplace_back(
    //         std::make_unique<Ball>(
    //                 pCircleShader.get(),
    //                 glm::vec3(200.0f, 846.8f, 50.0f),
    //                 glm::vec2(1,0)
    //         )
    // );    
}


void App::render()
{
    auto t = static_cast<float>(timeElapsedSinceLastFrame);

    // Update all shader uniforms.
    pTriangleShader->use();
    pTriangleShader->setFloat("windowWidth", kWindowWidth);
    pTriangleShader->setFloat("windowHeight", kWindowHeight);

    pCircleShader->use();
    pCircleShader->setFloat("windowWidth", kWindowWidth);
    pCircleShader->setFloat("windowHeight", kWindowHeight);

    // Render all shapes.
    for (auto & s : shapes)
    {
        s->render(t, animationEnabled);
    }
}
