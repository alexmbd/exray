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

    m_canvas = std::make_unique<Canvas>(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
    m_camera = Camera{Vector2{0.0f, 0.0f}, 1.0f};

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
    if (m_state == State::Select) { selectState(); }
    else
    {
        drawState();
    }
}

void Application::draw()
{
    BeginDrawing();
    ClearBackground(WHITE);
    m_canvas->draw();
    EndDrawing();
}

void Application::selectState()
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

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { m_selectedID = m_canvas->setSelectedShape(GetMousePosition()); }

    if (IsKeyReleased(KEY_TWO))
    {
        m_state = State::DrawRect;
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    }

    if (IsKeyReleased(KEY_THREE))
    {
        m_state = State::DrawDiamond;
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    }

    if (IsKeyReleased(KEY_FOUR))
    {
        m_state = State::DrawEllipse;
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    }

    if (IsKeyReleased(KEY_FIVE))
    {
        m_state = State::DrawArrowLine;
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    }

    if (IsKeyReleased(KEY_SIX))
    {
        m_state = State::DrawLine;
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
    }
}

void Application::drawState()
{
    if ((m_state == State::DrawArrowLine) || (m_state == State::DrawLine))
    {
        Vector2 worldMousePos = Vector2Scale(Vector2Subtract(GetMousePosition(), m_camera.target), 1.0f / m_camera.zoom);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && (m_drawAttr.id == 0))
        {
            m_drawAttr.id = m_canvas->createShape();
            m_drawAttr.points.emplace_back(worldMousePos);
            m_drawAttr.isDragging = true;
            Shape *shape          = m_canvas->shape(m_drawAttr.id);
            shape->tvgShape->strokeFill(0, 0, 0, 255);
            shape->tvgShape->strokeWidth(2.0f);
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            if ((m_drawAttr.points.size() != 1) || !m_drawAttr.isDragging) { m_drawAttr.points.emplace_back(worldMousePos); }
            else
            {
                float distance = Vector2Distance(worldMousePos, m_drawAttr.points[0]);
                if (distance < m_drawAttr.distanceThreshold) { m_drawAttr.isDragging = false; }
                else
                {
                    if (m_state == State::DrawArrowLine) { m_canvas->addArrowLine(m_drawAttr.id, m_drawAttr.points[0], worldMousePos); }
                    else
                    {
                        m_canvas->addLine(m_drawAttr.id, m_drawAttr.points[0], worldMousePos);
                    }
                    m_drawAttr.lineDone = true;
                    m_needToRedraw      = true;
                }
            }
        }

        if ((m_drawAttr.id != 0) && (!m_drawAttr.lineDone))
        {
            if (m_state == State::DrawArrowLine)
            {
                m_drawAttr.lineDone = m_canvas->addArrowLines(m_drawAttr.id, worldMousePos, m_drawAttr.points);
            }
            else
            {
                m_drawAttr.lineDone = m_canvas->addLines(m_drawAttr.id, worldMousePos, m_drawAttr.points);
            }
            m_needToRedraw = true;
        }
    }
    else
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 worldMousePos = Vector2Scale(Vector2Subtract(GetMousePosition(), m_camera.target), 1.0f / m_camera.zoom);
            if (m_drawAttr.id == 0)
            {
                m_drawAttr.id = m_canvas->createShape();
                m_drawAttr.points.emplace_back(worldMousePos);
                Shape *shape = m_canvas->shape(m_drawAttr.id);
                shape->tvgShape->strokeFill(0, 0, 0, 255);
                shape->tvgShape->strokeWidth(2.0f);
            }
            Vector2 pos  = Vector2Min(worldMousePos, m_drawAttr.points[0]);
            Vector2 size = Vector2Subtract(worldMousePos, m_drawAttr.points[0]);
            size         = Vector2{std::abs(size.x), std::abs(size.y)};

            if (m_state == State::DrawRect) { m_canvas->addRect(m_drawAttr.id, pos, size); }
            else if (m_state == State::DrawDiamond) { m_canvas->addDiamond(m_drawAttr.id, pos, size); }
            else if (m_state == State::DrawEllipse) { m_canvas->addEllipse(m_drawAttr.id, pos, size); }
            m_needToRedraw = true;
        }
    }

    if (m_needToRedraw)
    {
        m_canvas->update(m_camera);
        m_needToRedraw = false;
    }

    if (IsKeyReleased(KEY_ONE) || IsKeyReleased(KEY_ESCAPE) ||
        (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && (m_state != State::DrawArrowLine) && (m_state != State::DrawLine)) ||
        m_drawAttr.lineDone)
    {
        m_drawAttr.reset();
        m_canvas->update(m_camera);
        m_needToRedraw = false;
        m_state        = State::Select;
        SetMouseCursor(MOUSE_CURSOR_ARROW);
    }
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