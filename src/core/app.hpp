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
        DrawEllipse
    };

    Application(const Window &window);
    ~Application();

    void update();
    void draw();

  private:
    std::unique_ptr<Canvas> m_canvas;
    Camera m_camera;

    uint32_t m_tempId      = 0;
    Vector2 m_tempStartPos = {0};

    State m_state          = State::Select;
    bool m_needToRedraw    = false;
};

void run();
}