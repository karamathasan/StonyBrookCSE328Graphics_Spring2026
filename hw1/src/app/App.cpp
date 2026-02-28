#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/App.h"
#include "shape/Pixel.h"
#include "util/Shader.h"

#include <iostream>
#include <string>
std::string state = "mono-line";

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

    // Display a preview line which moves with the mouse cursor iff.
    // the most-recent mouse click is left click.
    // showPreview is controlled by mouseButtonCallback.
    if (app.showPreview)
    {
        auto pixel = dynamic_cast<Pixel *>(app.shapes.front().get());

        auto x0 = static_cast<int>(app.lastMouseLeftPressPos.x);
        auto y0 = static_cast<int>(app.lastMouseLeftPressPos.y);
        auto x1 = static_cast<int>(app.mousePos.x);
        auto y1 = static_cast<int>(app.mousePos.y);

        pixel->path.clear();
        bresenhamLine(pixel->path, x0, y0, x1, y1);
        pixel->dirty = true;
    }
}


void App::framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void App::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_1 && action == GLFW_RELEASE && state != "mono-line")
    {
        state = "mono-line";
        std::cout << state << std::endl;
        return;
    }

    if (key == GLFW_KEY_3 && action == GLFW_RELEASE && state != "poly-line")
    {
        state = "poly-line";
        std::cout << state << std::endl;
        return;
    }

    if (key == GLFW_KEY_4 && action == GLFW_RELEASE && state != "circle")
    {
        state = "circle";
        std::cout << state << std::endl;
        return;
    }

    //ellipse mode transitions
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS && state == "circle"){
        state = "ellipse";
        std::cout << state << std::endl;
        return;
    }

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE && state == "ellipse"){
        state = "circle";
        std::cout << state << std::endl;
        return;
    }
}


void App::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    // std::cout << "key: " << button << " action: " << action << std::endl;

    //use the callback to call the functions that are related to the specific state of the program. 
    //the method called will be based on the state. each method will take the button and action as parameters to decide what to do

    if (state == "mono-line"){
        monolineInput(window, button, action);
    }

    // input management for poly-line
    else if (state == "poly-line"){
        polylineInput(window, button, action);
    }

    // input management for circles
    else if (state == "circle"){
        circleInput(window, button, action);
    }

    // input management for ellipses
    else if (state == "ellipse"){
        ellipseInput(window, button, action);
    }
}


void App::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{}


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


void App::monolineInput(GLFWwindow * window, int button, int action){
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
    // the mouse buttons do different things based on what mode its in. we need the logic here to be seperate from the behavior
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            app.mousePressed = true;
            app.lastMouseLeftClickPos = app.mousePos;
            app.lastMouseLeftPressPos = app.mousePos;
        }
        else if (action == GLFW_RELEASE)
        {
            app.mousePressed = false;
            app.showPreview = true;

            #ifdef DEBUG_MOUSE_POS
            std::cout << "[ " << app.mousePos.x << ' ' << app.mousePos.y << " ]\n";
            #endif
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_RELEASE)
        {
            app.showPreview = false;
        }
    }
}

void App::polylineInput(GLFWwindow * window, int button, int action){
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            app.mousePressed = true;
            app.lastMouseLeftClickPos = app.mousePos;
            app.lastMouseLeftPressPos = app.mousePos;
        }
        else if (action == GLFW_RELEASE)
        {
            app.mousePressed = false;
            app.showPreview = true;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_RELEASE)
        {
            app.showPreview = false;
        }
    }
}

void App::circleInput(GLFWwindow * window, int button, int action){

}

void App::ellipseInput(GLFWwindow * window, int button, int action){

}

//extended version
void App::bresenhamLine(std::vector<Pixel::Vertex> & path, int x0, int y0, int x1, int y1)
{
    bool reflect = (std::abs(y1-y0) - std::abs(x1-x0)) > 0 ;
    if (reflect){
        int temp = y0;
        y0 = x0;
        x0 = temp;

        temp = y1;
        y1 = x1;
        x1 = temp;
    }

    //swap start and end
    if (x0 > x1){
        int temp = x0;
        x0 = x1;
        x1 = temp;

        temp = y0;
        y0 = y1;
        y1 = temp;
    }
    
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int p = 2 * dy - dx;
    int twoDy = 2 * dy;
    int twoDyMinusDx = 2 * (dy - dx);

    //flip +y for -slopes
    int posy = 1; // marks the direction of positive y
    
    if (y1-y0 < 0){
        posy = -1; //swaps the direction of positive y
    }

    int x = x0;
    int y = y0;

    while (x < x1)
    {
        if (reflect){
            path.emplace_back(y, x, 1.0f, 1.0f, 1.0f);
        }
        else path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);

        ++x;

        if (p < 0)
        {
            p += twoDy;
        }
        else
        {
            y += posy;
            p += twoDyMinusDx;
        }
    }
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glLineWidth(1.0f);
    glPointSize(1.0f);

    // Initialize shaders and objects-to-render;
    pPixelShader = std::make_unique<Shader>("src/shader/pixel.vert.glsl",
                                            "src/shader/pixel.frag.glsl");

    shapes.emplace_back(std::make_unique<Pixel>(pPixelShader.get()));
}


void App::render()
{
    // Update all shader uniforms.
    pPixelShader->use();
    pPixelShader->setFloat("windowWidth", kWindowWidth);
    pPixelShader->setFloat("windowHeight", kWindowHeight);

    // Render all shapes.
    for (auto & s : shapes)
    {
        s->render();
    }
}
