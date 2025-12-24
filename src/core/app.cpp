#include "app.hpp"

#include "raymath.h"

namespace Exray
{
Application::Application(const Window &window)
{
    tvg::Initializer::init();

    SetTargetFPS(window.frameRate);
    InitWindow(window.width, window.height, "Exray");

    m_canvas = std::make_unique<Canvas>(window.width, window.height);
    m_camera = Camera{Vector2{0.0f, 0.0f}, 1.0f};

    // TEMP
    uint32_t id1 = m_canvas->createShape();
    Shape *shape = m_canvas->shape(id1);
    shape->tvgShape->moveTo(0, 0);
    shape->tvgShape->lineTo(100, 100);
    shape->tvgShape->lineTo(0, 100);
    shape->tvgShape->lineTo(100, 0);
    shape->tvgShape->fill(255, 0, 0, 255);

    uint32_t id2  = m_canvas->createShape();
    Shape *shape2 = m_canvas->shape(id2);
    shape2->tvgShape->moveTo(200, 200);
    shape2->tvgShape->lineTo(300, 300);
    shape2->tvgShape->lineTo(200, 300);
    shape2->tvgShape->lineTo(300, 200);
    shape2->tvgShape->fill(0, 255, 0, 255);

    m_canvas->draw(m_camera);
}

Application::~Application()
{
    tvg::Initializer::term();
    CloseWindow();
}

void Application::update()
{
    if (float wheel = GetMouseWheelMove(); wheel != 0.0f)
    {
        Vector2 mousePos  = GetMousePosition();

        float oldZoom     = m_camera.zoom;
        float factor      = (wheel > 0) ? 1.1f : 1.0f / 1.1f;
        m_camera.zoom     = Clamp(m_camera.zoom * factor, 0.1f, 20.0f);

        m_camera.target.x = mousePos.x - (mousePos.x - m_camera.target.x) * (m_camera.zoom / oldZoom);
        m_camera.target.y = mousePos.y - (mousePos.y - m_camera.target.y) * (m_camera.zoom / oldZoom);
        m_needToRedraw    = true;
    }

    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        m_camera.target = Vector2Add(m_camera.target, GetMouseDelta());
        m_needToRedraw  = true;
    }

    if (m_needToRedraw)
    {
        m_canvas->draw(m_camera);
        m_needToRedraw = false;
    }
}

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