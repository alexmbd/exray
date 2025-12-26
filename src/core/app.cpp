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

    m_canvas = std::make_unique<Canvas>(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
    m_camera = Camera{Vector2{0.0f, 0.0f}, 1.0f};

    // TEMP
    // for (int i = 0; i < 500; i++)
    // {
    //     uint32_t id1  = m_canvas->createShape();
    //     Shape *shape1 = m_canvas->shape(id1);
    //     int value     = GetRandomValue(0, 1000);
    //     int value2    = GetRandomValue(0, 1000);
    //     shape1->tvgShape->moveTo(value, value2);
    //     shape1->tvgShape->lineTo(100 + value, 100 + value2);
    //     shape1->tvgShape->lineTo(0 + value, 100 + value2);
    //     shape1->tvgShape->lineTo(100 + value, 0 + value2);
    //     shape1->tvgShape->fill(255, value, value2, 255);
    // }

    m_canvas->update(m_camera);
}

Application::~Application()
{
    tvg::Initializer::term();
    CloseWindow();
}

void Application::update()
{
    SetWindowTitle(std::format("Exray - {} fps", GetFPS()).data());
    if (m_state == State::Select)
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
            m_canvas->update(m_camera);
            m_needToRedraw = false;
        }

        if (IsKeyReleased(KEY_TWO))
        {
            m_state = State::DrawRect;
            SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
        }
    }
    else if (m_state == State::DrawRect)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 mousePos      = GetMousePosition();
            Vector2 worldMousePos = {(mousePos.x - m_camera.target.x) / m_camera.zoom, (mousePos.y - m_camera.target.y) / m_camera.zoom};
            if (m_tempId == 0)
            {
                m_tempId       = m_canvas->createShape();
                m_tempStartPos = Vector2{worldMousePos.x, worldMousePos.y};
                Shape *shape   = m_canvas->shape(m_tempId);
                shape->tvgShape->strokeFill(0, 0, 0, 255);
                shape->tvgShape->strokeWidth(1.0f);
            }
            Vector2 pos  = {std::min(m_tempStartPos.x, worldMousePos.x), std::min(m_tempStartPos.y, worldMousePos.y)};
            Vector2 size = Vector2Subtract(worldMousePos, m_tempStartPos);

            Shape *shape = m_canvas->shape(m_tempId);
            shape->tvgShape->reset();
            shape->tvgShape->appendRect(pos.x, pos.y, std::abs(size.x), std::abs(size.y));
            m_needToRedraw = true;
        }

        if (m_needToRedraw)
        {
            m_canvas->update(m_camera);
            m_needToRedraw = false;
        }

        if ((IsKeyReleased(KEY_ONE)) || (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)))
        {
            m_tempId       = 0;
            m_tempStartPos = {0};
            m_canvas->update(m_camera);
            m_needToRedraw = false;
            m_state        = State::Select;
            SetMouseCursor(MOUSE_CURSOR_ARROW);
        }
    }
}

void Application::draw()
{
    BeginDrawing();
    ClearBackground(WHITE);
    m_canvas->draw();
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