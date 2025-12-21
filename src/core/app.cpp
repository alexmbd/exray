#include "app.hpp"
#include "canvas.hpp"

#include "raylib.h"

namespace Exray
{
Application::Application(const Window &window)
{
    tvg::Initializer::init();

    SetTargetFPS(window.frameRate);
    InitWindow(window.width, window.height, "Exray");

    m_canvas = std::make_unique<Canvas>(window.width, window.height);

    // TEMP
    Shape shape;
    shape.tvgShape = tvg::Shape::gen();
    shape.tvgShape->moveTo(0, 0);
    shape.tvgShape->lineTo(100, 100);
    shape.tvgShape->lineTo(0, 100);
    shape.tvgShape->lineTo(100, 0);
    shape.tvgShape->fill(255, 0, 0, 255);
    m_canvas->addShape(shape);
    m_canvas->draw();
}

Application::~Application()
{
    tvg::Initializer::term();
    CloseWindow();
}

void Application::update() {}

void Application::draw()
{
    BeginDrawing();
    ClearBackground(WHITE);
    DrawTexture(m_canvas->texture(), 0, 0, WHITE);
    EndDrawing();
}

void run()
{
    Application app(Window{900, 600, 60});
    while (!WindowShouldClose())
    {
        app.update();
        app.draw();
    }
}
}