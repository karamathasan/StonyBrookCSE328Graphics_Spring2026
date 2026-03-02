#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/App.h"
#include "shape/Pixel.h"
#include "util/Shader.h"

#include <iostream>
#include <string>
std::string state = "mono-line";

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
        if (state == "mono-line"){
            previewMonoline(app);
        }
        else if (state == "poly-line" || state == "poly-line-close"){
            previewPolyline(app);
        }
        else if (state == "circle"){
            previewCircle(app);
        }
        else if (state == "ellipse"){
            previewEllipse(app);
        }
        //parser is responsible for reading config.txt and then figuring out the appropriate thing to draw
        else if (state == "parser"){
            parse(app);
        }
    }
}

void App::previewMonoline(App & app){
    auto pixel = dynamic_cast<Pixel *>(app.shapes.front().get());

    auto x0 = static_cast<int>(app.lastMouseLeftPressPos.x);
    auto y0 = static_cast<int>(app.lastMouseLeftPressPos.y);
    auto x1 = static_cast<int>(app.mousePos.x);
    auto y1 = static_cast<int>(app.mousePos.y);

    pixel->path.clear();
    bresenhamLine(pixel->path, x0, y0, x1, y1);
    pixel->dirty = true;
}

void App::previewPolyline(App & app){
    auto pixel = dynamic_cast<Pixel *>(app.shapes.front().get());
    pixel->path.clear();

    for (int i = 0; i + 1 < app.polylinePoints.size(); i++){
        auto x0 = static_cast<int>(app.polylinePoints[i].x);
        auto y0  = static_cast<int>(app.polylinePoints[i].y);
        auto x1 = static_cast<int>(app.polylinePoints[i+1].x);
        auto y1 = static_cast<int>(app.polylinePoints[i+1].y);

        bresenhamLine(pixel->path, x0, y0, x1, y1);
    }

    if(app.endpoly){
        app.showPreview = false;
        // return;
    }
    else{
        auto x0 = static_cast<int>(app.lastMouseLeftPressPos.x);
        auto y0  = static_cast<int>(app.lastMouseLeftPressPos.y);
        auto x1 = static_cast<int>(app.mousePos.x);
        auto y1 = static_cast<int>(app.mousePos.y);
    
        bresenhamLine(pixel->path, x0, y0, x1, y1);
    }
    pixel->dirty = true;
}


void App::previewCircle(App & app){
    auto pixel = dynamic_cast<Pixel *>(app.shapes.front().get());

    auto x0 = static_cast<int>(app.lastMouseLeftPressPos.x);
    auto y0 = static_cast<int>(app.lastMouseLeftPressPos.y);
    auto x1 = static_cast<int>(app.mousePos.x);
    auto y1 = static_cast<int>(app.mousePos.y);

    pixel->path.clear();
    drawCircle(pixel->path, x0, y0, x1, y1);
    pixel->dirty = true;
}

void App::previewEllipse(App & app){
    auto pixel = dynamic_cast<Pixel *>(app.shapes.front().get());

    auto x0 = static_cast<int>(app.lastMouseLeftPressPos.x);
    auto y0 = static_cast<int>(app.lastMouseLeftPressPos.y);
    auto x1 = static_cast<int>(app.mousePos.x);
    auto y1 = static_cast<int>(app.mousePos.y);

    pixel->path.clear();
    drawEllipse(pixel->path, x0, y0, x1, y1);
    pixel->dirty = true;
}


void App::parse(App & app){
    auto pixel = dynamic_cast<Pixel *>(app.shapes.front().get());

    pixel->path.clear();
    // app.drawQuadratic(pixel->path, 1,1,1);
    // // app.drawCircle(pixel->path, 500,500,700,700);
    // pixel->dirty = true;

    // return;
    std::string fp = "etc/config.txt";
    std::ifstream file(fp);

    if (!file.is_open()){
        std::cerr << "error opening file" << std::endl;
        return;
    }

    std::string line;
    if (!std::getline(file, line)){
        std::cerr << "file is empty" << std::endl;   
    }
    
    file.close();

    std::stringstream ss(line);
    std::string type;
    std::getline(ss, type, ',');

    if (type == "quadratic" || type == "1"){
        double a, b, c;

        if (ss >> a && ss.ignore(1) &&
            ss >> b && ss.ignore(1) &&
            ss >> c) {

            std::cout << "Quadratic: "
                      << a << ", " << b << ", " << c << "\n";
            // app.setQuadratic(a, b, c);
            // state = "parse-quadratic";
            drawQuadratic(pixel->path, a, b, c);
            pixel->dirty = true;

        } else {
            std::cerr << "Invalid quadratic parameters\n";
        }
        
    }
    else if (type == "cubic" || type == "2"){
        double a, b, c, d;

        if (ss >> a && ss.ignore(1) &&
            ss >> b && ss.ignore(1) &&
            ss >> c && ss.ignore(1) &&
            ss >> d) {

            // std::cout << "Cubic: "
            //           << a << ", " << b << ", "
            //           << c << ", " << d << "\n";

            // state = "parse-cubic";
            drawCubic(pixel->path, a, b,c, d);
            pixel->dirty = true;
                    
        } else {
            std::cerr << "Invalid cubic parameters\n";
        }
    }
    else if (type == "super-quadric" || type == "3"){
        int n;
        double a, b;

        if (ss >> n && ss.ignore(1) &&
            ss >> a && ss.ignore(1) &&
            ss >> b) {

            // std::cout << "Super-Quadric: "
            //           << n << ", "
            //           << a << ", "
            //           << b << "\n";

            // state = "parse-super-quadric";
            drawSuperQuadric(pixel->path, n, a, b);
            pixel->dirty = true;


        } else {
            std::cerr << "Invalid super-quadric parameters\n";
        }
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
        return;
    }

    if (key == GLFW_KEY_3 && action == GLFW_RELEASE && state != "poly-line")
    {
        state = "poly-line";
        return;
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS && state == "poly-line")
    {
        state = "poly-line-close";
        return;
    }

    if (key == GLFW_KEY_C && action == GLFW_RELEASE && state == "poly-line-close")
    {
        state = "poly-line";
        return;
    }

    if (key == GLFW_KEY_4 && action == GLFW_RELEASE && state != "circle")
    {
        state = "circle";
        return;
    }

    //ellipse mode transitions
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS && state == "circle"){
        state = "ellipse";
        return;
    }

    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE && state == "ellipse"){
        state = "circle";
        return;
    }

    //bonus
    if (key == GLFW_KEY_5 && action == GLFW_RELEASE && state != "parser"){
        state = "parser";
        return;
    }
}


void App::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    //use the callback to call the functions that are related to the specific state of the program. 
    //the method called will be based on the state. each method will take the button and action as parameters to decide what to do

    if (state == "mono-line"){
        monolineInput(window, button, action);
    }

    // input management for poly-line
    else if (state == "poly-line"){
        polylineInput(window, button, action);
    }

    else if (state == "poly-line-close"){
        polylineClose(window, button, action);
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
        auto pixel = dynamic_cast<Pixel *>(app.shapes.front().get());

        if (action == GLFW_PRESS)
        {
            if (app.endpoly){
                app.polylinePoints.clear();
                app.endpoly = false;
            }
            app.mousePressed = true;
            app.lastMouseLeftClickPos = app.mousePos;
            app.lastMouseLeftPressPos = app.mousePos;
        }
        else if (action == GLFW_RELEASE)
        {
            app.polylinePoints.push_back(app.mousePos);
            app.mousePressed = false;
            app.showPreview = true;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_RELEASE)
        {
            app.showPreview = false;
            app.endpoly = true;
        }
    }
}

void App::polylineClose(GLFWwindow * window, int button, int action){
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_RELEASE)
        {
            if (app.polylinePoints.size() > 1){
                app.polylinePoints.push_back(app.polylinePoints.at(0));
            }
            app.endpoly = true;
        }
    }
}

void App::circleInput(GLFWwindow * window, int button, int action){
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

void App::ellipseInput(GLFWwindow * window, int button, int action){
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

void App::drawCircle(std::vector<Pixel::Vertex> & path, int x0, int y0, int x1, int y1){
    int dx = x1 - x0;
    int dy = y1 - y0;
    int lensqred = dx * dx + dy * dy;
    double r = std::sqrt((double)lensqred);

    r = (int)std::round(r);

    int x = 0;
    int y = r;
    
    double p = (5.0 / 4) - r;
    while(x <= y){
        path.emplace_back( x + x0, y + y0,1.0,1.0,1.0);
        path.emplace_back( x + x0,-y + y0,1.0,1.0,1.0);
        path.emplace_back(-x + x0, y + y0,1.0,1.0,1.0);
        path.emplace_back(-x + x0,-y + y0,1.0,1.0,1.0);
        
        path.emplace_back( y + x0, x + y0,1.0,1.0,1.0);
        path.emplace_back( y + x0,-x + y0,1.0,1.0,1.0);
        path.emplace_back(-y + x0, x + y0,1.0,1.0,1.0);
        path.emplace_back(-y + x0,-x + y0,1.0,1.0,1.0);

        x++;
        if (p < 0){
            p = p + 2 * x + 1;
        } else {
            y--;
            p = p + 2 * x + 1 - 2 * y;
        }
    }
    
}

void App::drawEllipse(std::vector<Pixel::Vertex> & path, int x0, int y0, int x1, int y1){
    int rx = std::abs(x1-x0);
    int ry = std::abs(y1-y0);

    long long rx2 = rx * rx;
    long long ry2 = ry * ry;

    long long twoRx2 = 2 * rx2;
    long long twoRy2 = 2 * ry2;

    long long x = 0;
    long long y = ry;
    
    long long p1 = ry2 - rx2 * ry + rx2 / 4;

    long long px = 0;
    long long py = twoRx2 * y;
    while (px < py){
        path.emplace_back(x0 + x,y0 + y,1,1,1);
        path.emplace_back(x0 - x,y0 + y,1,1,1);
        path.emplace_back(x0 + x,y0 - y,1,1,1);
        path.emplace_back(x0 - x,y0 - y,1,1,1);
        x++;
        px += twoRy2;
        if (p1 < 0){
            p1 += ry2 + px;
        }
        else {
            y--;
            py -= twoRx2;
            p1 += ry2 + px - py;
        }
    }

    long long p2 = ry2 * (x * x + x) + ry2 / 4 + rx2 * (y-1) * (y-1) - rx2 * ry2;
    while (y >= 0){
        path.emplace_back(x0 + x,y0 + y,1,1,1);
        path.emplace_back(x0 - x,y0 + y,1,1,1);
        path.emplace_back(x0 + x,y0 - y,1,1,1);
        path.emplace_back(x0 - x,y0 - y,1,1,1);

        y--;
        py -= twoRx2;
        if (p2>0){
            p2 += rx2 - py;
        }
        else {
            x++;
            px += twoRy2;
            p2 += rx2 - py + px;
        }
    }
}

// void App::drawQuadratic(std::vector<Pixel::Vertex>& path, double a, double b, double c) {
//     // We iterate over x and use the midpoint decision to determine
//     // whether to step in y or stay, handling steep/shallow regions.
    
//     // Evaluate the curve across the window width
//     // y = ax^2 + bx + c
//     // dy/dx = 2ax + b
    
//     auto F = [&](double x, double y) -> double {
//         // Implicit form: F(x,y) = ax^2 + bx + c - y = 0
//         return a * x * x + b * x + c - y;
//     };
    
//     int x = 0;
//     int y = (int)std::round(a * 0 * 0 + b * 0 + c); // starting y at x=0
    
//     // Clamp starting y to window
//     y = std::max(0, std::min((int)App::kWindowHeight, y));
    
//     path.emplace_back(x + App::kWindowWidth/2, y + App::kWindowHeight/2, 1, 1, 1);
    
//     for (x = 1; x <= (int)App::kWindowWidth; ++x) {
//         double slope = 2.0 * a * (x - 1) + b; // dy/dx at previous x
        
//         if (std::abs(slope) <= 1.0) {
//             // Shallow region: step x by 1, decide y using midpoint
//             // Midpoint is at (x, y + 0.5) — should we increment y?
//             double mid = F(x, y + 0.5);
//             if (a >= 0) {
//                 // Concave up: increment y if midpoint is below curve
//                 if (mid < 0) y += 1;
//             } else {
//                 // Concave down: decrement y if midpoint is above curve
//                 if (mid > 0) y -= 1;
//             }
//         } else {
//             // Steep region: need to fill multiple y pixels for this x
//             int y_exact = (int)std::round(a * x * x + b * x + c);
//             // Fill vertically from previous y to new y
//             int y_step = (y_exact > y) ? 1 : -1;
//             while (y != y_exact) {
//                 y += y_step;
//                 path.emplace_back(x + App::kWindowWidth/2, y + App::kWindowHeight/2, 1, 1, 1);
//             }
//         }
        
//         path.emplace_back(x + App::kWindowWidth/2, y + App::kWindowHeight/2, 1, 1, 1);

//     }
// }

void App::drawQuadratic(std::vector<Pixel::Vertex>& path, double a, double b, double c) {

    int x0 = std::round(- b / (2 * a));
    int x = x0; // start at the axis of symmetry
    int y = std::round(a * x * x + b * x + c);

    int concaveup = a > 0 ? 1 : -1;
    
    // Clamp starting y to window
    y = std::max(0, std::min((int)App::kWindowHeight, y));

    double slope = 2 * a * x + b; 
    double p1 = a * (x+1) * (x+1) + b * (x+1) + c - (y + 0.5 * concaveup);
    
    while (std::abs(slope) < 1){
        x++;
        // if (concaveup * p1 < 0){
        if (concaveup * p1 > 0){
            y += concaveup;
            p1 -= concaveup;
        }
        p1 += a * (2 * x + 1 )+ b;
        slope += 2 * a;

        path.emplace_back(x + App::kWindowWidth/2, y + App::kWindowHeight/2,1,1,1);
        path.emplace_back(-x + App::kWindowWidth/2, y + App::kWindowHeight/2,1,1,1);
    }

    while (y < App::kWindowHeight/2){
        y += concaveup;
        if (concaveup * p1 < 0){
            x += 1;
            p1 += a * (2 * x + 1 )+ b; 
        }
        p1 -= concaveup;

        path.emplace_back(x + App::kWindowWidth/2, y + App::kWindowHeight/2,1,1,1);
        path.emplace_back(-x + App::kWindowWidth/2, y + App::kWindowHeight/2,1,1,1);
    }
}

void App::drawCubic(std::vector<Pixel::Vertex> & path, double a, double b, double c, double d){
    auto F = [&](double x, double y) -> double {
        return a * x * x * x + b * x * x + c * x + d - y;
    };

    auto dydx = [&](double x) -> double {
        return 3.0 * a * x * x + 2.0 * b * x + c;
    };

    int x = 0;
    int y = (int)std::round(d); // y at x=0 is just d
    y = std::max(0, std::min((int)App::kWindowHeight, y));

    path.emplace_back(x + App::kWindowWidth/2, y + App::kWindowHeight/2, 1, 1, 1);

    for (x = 1; x <= (int)App::kWindowWidth; ++x) {
        double slope = dydx(x - 1);

        if (std::abs(slope) <= 1.0) {
            // Shallow: use midpoint to decide y adjustment
            // Evaluate midpoint at (x, y+0.5) and (x, y-0.5)
            double mid_up   = F(x, y + 0.5);
            double mid_down = F(x, y - 0.5);

            // The curve is above us if F(x,y) > 0, below if F(x,y) < 0
            // We want to track the zero crossing
            double fCurrent = F(x, (double)y);
            if (fCurrent > 0.5)       y += 1;
            else if (fCurrent < -0.5) y -= 1;

        } else {
            // Steep: walk y to close the gap, plotting each step
            int y_exact = (int)std::round(a * x * x * x + b * x * x + c * x + d);
            y_exact = std::max(0, std::min((int)App::kWindowHeight/2, y_exact));

            int y_step = (y_exact > y) ? 1 : -1;
            while (y != y_exact) {
                y += y_step;
                path.emplace_back(x + App::kWindowWidth/2, y + App::kWindowHeight/2, 1, 1, 1);
            }
        }

        path.emplace_back(x + App::kWindowWidth/2, y + App::kWindowHeight/2, 1, 1, 1);
    }
}

void App::drawSuperQuadric(std::vector<Pixel::Vertex> & path, int n, double a, double b){

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
