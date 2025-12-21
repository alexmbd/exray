#pragma once

#include "raylib.h"
#include "thorvg.h"

#include <memory>
#include <vector>

namespace Exray
{
struct Shape
{
    tvg::Shape *tvgShape;
    Vector2 position      = {0.0f, 0.0f};
    Vector2 scale         = {1.0f, 1.0f};
    Rectangle localBounds = {0.0f, 0.0f, 0.0f, 0.0f};
};

class Canvas
{
  public:
    Canvas(int32_t width, int32_t height);

    void draw();
    void addShape(const Shape &shape);
    const Texture2D &texture() const;

  private:
    tvg::SwCanvas *m_tvgCanvas;
    std::vector<Shape> m_shapes;
    int32_t m_width     = 0;
    int32_t m_height    = 0;
    Image m_image       = {0};
    Texture2D m_texture = {0};
};
}