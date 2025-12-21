#pragma once

#include <memory>

namespace Exray
{
class Canvas;

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
};

void run();
}