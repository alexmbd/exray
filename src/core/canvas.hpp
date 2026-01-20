#pragma once

#include "raylib.h"
#include "thorvg.h"

#include <memory>
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
    Rectangle bounds;     // Used for hit testing
};

inline constexpr uint32_t INVALID_ID = 0;

class Canvas
{
  public:
    explicit Canvas(int32_t width, int32_t height);
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

    void addArrowLine(uint32_t id, const Vector2 &posA, const Vector2 &posB);
    // Returns true if the lines create a closed shape (points[0] == pos approximately) and false otherwise
    bool addArrowLines(uint32_t id, const Vector2 &pos, const std::vector<Vector2> &points);

    void addLine(uint32_t id, const Vector2 &posA, const Vector2 &posB);
    // Returns true if the lines create a closed shape (points[0] == pos approximately) and false otherwise
    bool addLines(uint32_t id, const Vector2 &pos, const std::vector<Vector2> &points);

    // Returns the id of the first shape that pos is contained. Otherwise, it returns 0 (an invalid id)
    uint32_t setSelectedShape(const Vector2 &pos);
    // Returns a non-owning pointer or a nullptr if nothing is selected
    Shape *selectedShape();

  private:
    tvg::SwCanvas *m_tvgCanvas;
    std::vector<Shape> m_shapes;

    uint32_t m_currentID  = INVALID_ID; // ID used for Shape creation
    uint32_t m_selectedID = INVALID_ID; // ID of the selected Shape

    float m_lineThreshold = 2.0f;

    Image m_image         = {0};
    Texture2D m_texture   = {0};
};

// Basically, a Canvas plus Input (Mouse and Keyboard) Handling
class CanvasHandler
{
  public:
    enum class State
    {
        Select,
        DrawRect,
        DrawDiamond,
        DrawEllipse,
        DrawArrowLine,
        DrawLine
    };

    CanvasHandler();
    ~CanvasHandler() = default;

    void update();
    void draw();

  private:
    struct DrawAttributes
    {
        std::vector<Vector2> points;
        bool lineDone           = false;
        bool isDragging         = false;
        uint32_t id             = INVALID_ID;
        float distanceThreshold = 8.0f;

        inline void reset()
        {
            points.clear();
            lineDone   = false;
            isDragging = false;
            id         = INVALID_ID;
        }
    };

    std::unique_ptr<Canvas> m_canvas;
    Camera m_camera;
    DrawAttributes m_drawAttr;

    uint32_t m_selectedID = INVALID_ID;
    bool m_needToRedraw   = false;
    State m_state         = State::Select;

    void selectState();
    void drawState();
};
}