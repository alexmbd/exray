#pragma once

#include "canvas.hpp"

#include <memory>

namespace Exray
{
struct Window
{
    int32_t width;
    int32_t height;
    int32_t frameRate;
};

class Application
{
  public:
    enum class State
    {
        Select,
        DrawRect,
        DrawDiamond,
        DrawEllipse,
        DrawLine
    };

    Application(const Window &window);
    ~Application();

    void update();
    void draw();

  private:
    struct DrawAttributes
    {
        std::vector<Vector2> points;
        bool lineDone           = false;
        bool isDragging         = false;
        uint32_t id             = 0;
        float distanceThreshold = 8.0f;

        inline void reset()
        {
            points.clear();
            lineDone   = false;
            isDragging = false;
            id         = 0;
        }
    };

    std::unique_ptr<Canvas> m_canvas;
    Camera m_camera;

    DrawAttributes m_drawAttr;

    State m_state       = State::Select;
    bool m_needToRedraw = false;

    void selectState();
    void drawState();
};

void run();
}