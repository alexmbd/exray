#include "canvas.hpp"

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
}