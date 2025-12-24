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
    Application(const Window &window);
    ~Application();

    void update();
    void draw();

  private:
    std::unique_ptr<Canvas> m_canvas;
    Camera m_camera;
    bool m_needToRedraw = false;
};

void run();
}