#include"renderer.h"
#include"mesh.h"

#include<utility>

sk::renderer::renderer(TGAImage& target) : m_image(target) {}

sk::renderer::~renderer()
{
    m_image.flip_vertically();
    m_image.write_tga_file("output.tga");
}

void sk::renderer::draw(const sk::point& p, const TGAColor& c)
{
    if (p.x < 0 || p.x > m_image.get_width()) return;
    if (p.y < 0 || p.y > m_image.get_height()) return;

    m_image.set(p.x, p.y, c);
}

void sk::renderer::draw_line(int x0,int y0,int x1,int y1, const TGAColor& c)
{
    sk::line l(x0, y0, x1, y1);
    draw(l, c);
}

void sk::renderer::draw(const sk::line& l, const TGAColor& c)
{
    auto point = l.start();
    auto err = 0;

    while (point.x != l.end().x)
    {
        l.steep() ? m_image.set(point.y, point.x, c) : m_image.set(point.x, point.y, c);

        point = get_next_line_point(l, point, err);
    }
}

void sk::renderer::draw_wireframe(const model& m, const TGAColor& c)
{
    for (const auto& f : m.faces)
    {
    const auto& v0 = m.verts[f[0]];
    const auto& v1 = m.verts[f[1]];
    const auto& v2 = m.verts[f[2]];
        draw_triangle(v0, v1, v2, c);
    }
}

void sk::renderer::draw_triangle(
        const sk::vec3f& v0,
        const sk::vec3f& v1,
        const sk::vec3f& v2,
        const TGAColor& c)
{
    draw_line(v0, v1, c);
    draw_line(v1, v2, c);
    draw_line(v2, v0, c);
}

void sk::renderer::draw_line(const sk::vec3f& v0, const sk::vec3f& v1, const TGAColor& c)
{
    auto image_v0 = from_ndc(v0), image_v1 = from_ndc(v1);
    draw_line(image_v0[0], image_v0[1], image_v1[0], image_v1[1], c);
}


void sk::renderer::draw_filled_triangle(
    const sk::vec3f& v0,
    const sk::vec3f& v1,
    const sk::vec3f& v2,
    const TGAColor& c)
{
    if (v0[1] == v1[1] && v0[1] == v2[1] || v0[0] == v1[0] && v0[0] == v2[0]) return;

    sk::triangle t(from_ndc(v0), from_ndc(v1), from_ndc(v2));

    sk::bounding_box bb(t);

    for (const auto& p : bb)
    {
        if (!t.contains(p)) continue;

        m_image.set(p.x, p.y, c);
    }
}

sk::vec3i sk::renderer::from_ndc(const sk::vec3f& v)
{
    sk::vec3i res;
    res[0] = (v[0] + 1.) * m_image.get_width() / 2.;
    res[1] = (v[1] + 1.) * m_image.get_height() / 2.;

    return res;
}

sk::point sk::renderer::get_next_line_point(const sk::line& l, const point& prev_point, int& err)
{
    if (prev_point.x == l.end().x) return prev_point;

    auto x = prev_point.x + 1;
    auto y = prev_point.y;

    err += l.derr();
    if (err >= l.dx())
    {
         y += l.dy() > 0 ? 1 : -1;
         err -= 2 * l.dx();
    }

    return sk::point(x,y);
}
