#include "canvas.hpp"

#include "raymath.h"

namespace Exray
{
Canvas::Canvas(int32_t width, int32_t height)
{
    m_image        = GenImageColor(width, height, BLANK);
    m_image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    m_texture      = LoadTextureFromImage(m_image);
    m_tvgCanvas    = tvg::SwCanvas::gen();
    m_tvgCanvas->target((uint32_t *)m_image.data, width, width, height, tvg::ColorSpace::ABGR8888S);

    // The first item of the vector is always the background rect
    uint32_t id       = createShape();
    Shape *background = shape(id);
    background->tvgShape->appendRect(0.0f, 0.0f, width, height);
    background->tvgShape->fill(255, 255, 255, 255);
}

Canvas::~Canvas()
{
    UnloadTexture(m_texture);
    delete m_tvgCanvas;
}

void Canvas::update(const Camera &camera)
{
    for (Shape &shape : m_shapes)
    {
        shape.tvgShape->translate(camera.target.x, camera.target.y);
        shape.tvgShape->scale(camera.zoom);
        shape.tvgShape->bounds(&shape.bounds.x, &shape.bounds.y, &shape.bounds.width, &shape.bounds.height);
        m_tvgCanvas->push(shape.tvgShape);
    }
    m_tvgCanvas->draw();
    m_tvgCanvas->sync();

    UpdateTexture(m_texture, m_image.data);
}

void Canvas::draw() { DrawTexture(m_texture, 0, 0, WHITE); }

uint32_t Canvas::createShape()
{
    m_currentID++;
    m_shapes.emplace_back(Shape{tvg::Shape::gen(), m_currentID, Rectangle{0.0f, 0.0f, 0.0f, 0.0f}});
    return m_currentID;
}

Shape *Canvas::shape(uint32_t id)
{
    for (std::size_t i = 0; i < m_shapes.size(); i++)
    {
        Shape *shape = &m_shapes[i];
        if (shape->id == id) { return shape; }
    }
    return nullptr;
}

void Canvas::addRect(uint32_t id, const Vector2 &pos, const Vector2 &size)
{
    Shape *s = shape(id);
    s->tvgShape->reset();
    Vector2 topRight    = {pos.x + size.x, pos.y};
    Vector2 bottomRight = Vector2Add(pos, size);
    // The y value of bottomLeft has a (+ 0.025f) term as a workaround
    // to prevent false detecting the inside of the rect when it's not filled
    Vector2 bottomLeft = {pos.x, pos.y + size.y + 0.025f};
    s->tvgShape->moveTo(pos.x, pos.y);
    s->tvgShape->lineTo(topRight.x, topRight.y);
    s->tvgShape->lineTo(bottomRight.x, bottomRight.y);
    s->tvgShape->lineTo(bottomLeft.x, bottomLeft.y);
    s->tvgShape->close();
}

void Canvas::addDiamond(uint32_t id, const Vector2 &pos, const Vector2 &size)
{
    Shape *s = shape(id);
    s->tvgShape->reset();
    Vector2 radius       = Vector2Scale(size, 0.5f);
    Vector2 topCenter    = {pos.x + radius.x, pos.y};
    Vector2 bottomCenter = {pos.x + radius.x, pos.y + size.y};
    Vector2 leftCenter   = {pos.x, pos.y + radius.y};
    Vector2 rightCenter  = {pos.x + size.x, pos.y + radius.y};
    s->tvgShape->moveTo(topCenter.x, topCenter.y);
    s->tvgShape->lineTo(rightCenter.x, rightCenter.y);
    s->tvgShape->lineTo(bottomCenter.x, bottomCenter.y);
    s->tvgShape->lineTo(leftCenter.x, leftCenter.y);
    s->tvgShape->close();
}

void Canvas::addEllipse(uint32_t id, const Vector2 &pos, const Vector2 &size)
{
    Shape *s = shape(id);
    s->tvgShape->reset();
    Vector2 radius = Vector2Scale(size, 0.5f);
    Vector2 center = {pos.x + radius.x, pos.y + radius.y};
    s->tvgShape->appendCircle(center.x, center.y, radius.x, radius.y);
}

void Canvas::addArrowLine(uint32_t id, const Vector2 &posA, const Vector2 &posB)
{
    Shape *s          = shape(id);

    float headLength  = 12.0f;
    float headWidth   = 8.0f;
    Vector2 direction = Vector2Normalize(Vector2Subtract(posA, posB));
    Vector2 perp      = {-direction.y, direction.x};
    Vector2 point     = Vector2Add(posB, Vector2Scale(direction, headLength));
    Vector2 posC      = Vector2Add(point, Vector2Scale(perp, headWidth));
    Vector2 posD      = Vector2Subtract(point, Vector2Scale(perp, headWidth));

    s->tvgShape->reset();
    s->tvgShape->strokeCap(tvg::StrokeCap::Round);
    s->tvgShape->lineTo(posA.x, posA.y);
    s->tvgShape->lineTo(posB.x, posB.y);

    s->tvgShape->moveTo(posC.x, posC.y);
    s->tvgShape->lineTo(posB.x, posB.y);
    s->tvgShape->moveTo(posD.x, posD.y);
    s->tvgShape->lineTo(posB.x, posB.y);
}

bool Canvas::addArrowLines(uint32_t id, const Vector2 &pos, const std::vector<Vector2> &points)
{
    Shape *s = shape(id);
    s->tvgShape->reset();
    s->tvgShape->strokeCap(tvg::StrokeCap::Round);

    for (const Vector2 &point : points) { s->tvgShape->lineTo(point.x, point.y); }

    const Vector2 &lastPos = points.back();
    float headLength       = 12.0f;
    float headWidth        = 8.0f;
    Vector2 direction      = Vector2Normalize(Vector2Subtract(lastPos, pos));
    Vector2 perp           = {-direction.y, direction.x};
    Vector2 point          = Vector2Add(pos, Vector2Scale(direction, headLength));
    Vector2 posC           = Vector2Add(point, Vector2Scale(perp, headWidth));
    Vector2 posD           = Vector2Subtract(point, Vector2Scale(perp, headWidth));

    Vector2 diff           = Vector2Subtract(points[0], pos);
    if ((points.size() >= 3) && (std::abs(diff.x) < m_lineThreshold) && (std::abs(diff.y) < m_lineThreshold))
    {
        s->tvgShape->close();
        s->tvgShape->moveTo(posC.x, posC.y);
        s->tvgShape->lineTo(pos.x, pos.y);
        s->tvgShape->moveTo(posD.x, posD.y);
        s->tvgShape->lineTo(pos.x, pos.y);
        return true;
    }
    else
    {
        s->tvgShape->lineTo(pos.x, pos.y);
        s->tvgShape->moveTo(posC.x, posC.y);
        s->tvgShape->lineTo(pos.x, pos.y);
        s->tvgShape->moveTo(posD.x, posD.y);
        s->tvgShape->lineTo(pos.x, pos.y);
        return false;
    }
}

void Canvas::addLine(uint32_t id, const Vector2 &posA, const Vector2 &posB)
{
    Shape *s = shape(id);
    s->tvgShape->reset();
    s->tvgShape->strokeCap(tvg::StrokeCap::Round);
    s->tvgShape->lineTo(posA.x, posA.y);
    s->tvgShape->lineTo(posB.x, posB.y);
}

bool Canvas::addLines(uint32_t id, const Vector2 &pos, const std::vector<Vector2> &points)
{
    Shape *s = shape(id);
    s->tvgShape->reset();
    s->tvgShape->strokeCap(tvg::StrokeCap::Round);

    for (const Vector2 &point : points) { s->tvgShape->lineTo(point.x, point.y); }

    Vector2 diff = Vector2Subtract(points[0], pos);
    if ((points.size() >= 3) && (std::abs(diff.x) < m_lineThreshold) && (std::abs(diff.y) < m_lineThreshold))
    {
        s->tvgShape->close();
        return true;
    }
    else
    {
        s->tvgShape->lineTo(pos.x, pos.y);
        return false;
    }
}

uint32_t Canvas::shapeHasPoint(const Vector2 &pos)
{
    // The first item of the vector is always the background rect
    for (std::size_t i = m_shapes.size() - 1; i > 0; i--)
    {
        const Shape &s = m_shapes[i];
        if (s.tvgShape->intersects(pos.x, pos.y)) { return s.id; }
    }
    return 0;
}
}