#pragma once

#include "raylib.h"
#include "thorvg.h"

#include <vector>

namespace Exray
{
struct Camera
{
    Vector2 target;
    float zoom;
};

struct Shape
{
    tvg::Shape *tvgShape; // Non-owning pointer
    uint32_t id;          // Set by the Canvas
};

class Canvas
{
  public:
    Canvas(int32_t width, int32_t height);
    ~Canvas();

    void update(const Camera &camera);
    void draw();

    // Returns the id of the created Shape
    uint32_t createShape();
    // Returns a non-owning pointer or a nullptr if Shape.id is not in the Canvas
    Shape *shape(uint32_t id);

    void addRect(uint32_t id, const Vector2 &pos, const Vector2 &size);
    void addDiamond(uint32_t id, const Vector2 &pos, const Vector2 &size);
    void addEllipse(uint32_t id, const Vector2 &pos, const Vector2 &size);

  private:
    tvg::SwCanvas *m_tvgCanvas;
    std::vector<Shape> m_shapes;

    uint32_t m_currentID = 0;

    Image m_image        = {0};
    Texture2D m_texture  = {0};
};
}