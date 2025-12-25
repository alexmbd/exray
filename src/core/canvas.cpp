#include "canvas.hpp"

namespace Exray
{
Canvas::Canvas(int32_t width, int32_t height) : m_width(width), m_height(height)
{
    m_image     = GenImageColor(m_width, m_height, BLANK);
    m_texture   = LoadTextureFromImage(m_image);
    m_tvgCanvas = tvg::SwCanvas::gen();
    m_tvgCanvas->target((uint32_t *)m_image.data, m_width, m_width, m_height, tvg::ColorSpace::ARGB8888);
}

Canvas::~Canvas()
{
    UnloadTexture(m_texture);
    delete m_tvgCanvas;
}

void Canvas::draw(const Camera &camera)
{
    for (const Shape &shape : m_shapes)
    {
        shape.tvgShape->translate(camera.target.x, camera.target.y);
        shape.tvgShape->scale(camera.zoom);
        m_tvgCanvas->push(shape.tvgShape);
    }
    m_tvgCanvas->draw();
    m_tvgCanvas->sync();

    std::vector<uint8_t> rgba(m_width * m_height * 4);
    uint32_t *buffer = (uint32_t *)m_image.data;

    for (int32_t i = 0; i < m_width * m_height; i++)
    {
        uint32_t p = buffer[i];

        uint8_t a  = (p >> 24) & 0xFF;
        uint8_t r  = (p >> 16) & 0xFF;
        uint8_t g  = (p >> 8) & 0xFF;
        uint8_t b  = p & 0xFF;

        if (a > 0)
        {
            r = (r * 255) / a;
            g = (g * 255) / a;
            b = (b * 255) / a;
        }

        rgba[i * 4 + 0] = r;
        rgba[i * 4 + 1] = g;
        rgba[i * 4 + 2] = b;
        rgba[i * 4 + 3] = a;
    }

    UpdateTexture(m_texture, rgba.data());
}

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

const Texture2D &Canvas::texture() const { return m_texture; }
}