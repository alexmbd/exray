#include "app.hpp"

#include "raymath.h"

#include <format>

namespace Exray
{
Application::Application(const Window &window)
{
    tvg::Initializer::init();

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(window.frameRate);
    InitWindow(window.width, window.height, "Exray");
    SetExitKey(KEY_NULL);

    m_canvasHandler = std::make_unique<CanvasHandler>();
}

Application::~Application()
{
    tvg::Initializer::term();
    CloseWindow();
}

void Application::update()
{
    SetWindowTitle(std::format("Exray - {} fps", GetFPS()).data());
    m_canvasHandler->update();
}

void Application::draw()
{
    BeginDrawing();
    ClearBackground(WHITE);
    m_canvasHandler->draw();
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