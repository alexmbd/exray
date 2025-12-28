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
    for (const Shape &shape : m_shapes)
    {
        shape.tvgShape->translate(camera.target.x, camera.target.y);
        shape.tvgShape->scale(camera.zoom);
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
    m_shapes.emplace_back(Shape{tvg::Shape::gen(), m_currentID});
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
    s->tvgShape->appendRect(pos.x, pos.y, size.x, size.y);
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
}