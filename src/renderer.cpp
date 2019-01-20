#include "renderer.h"
#include "mesh.h"

#include <utility>
#include <limits>

sk::renderer::renderer(TGAImage& target) : m_image(target), m_z_buffer(m_image.get_height() * m_image.get_width(), -std::numeric_limits<float>::max()) {}

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
    const sk::vertex& v0,
    const sk::vertex& v1,
    const sk::vertex& v2,
    TGAImage* texture)
{
    if (v0.pos[1] == v1.pos[1] && v0.pos[1] == v2.pos[1] || v0.pos[0] == v1.pos[0] && v0.pos[0] == v2.pos[0]) return;

    sk::triangle t(from_ndc(v0.pos), from_ndc(v1.pos), from_ndc(v2.pos));

    sk::bounding_box bb(t, m_image);

    for (auto p : bb)
    {
        auto barycentric = t.barycentric(p);

        if (barycentric[0] < 0 || barycentric[1] < 0 || barycentric[2] < 0) continue;

        p.z = t.v0[2] * barycentric[0] + t.v1[2] * barycentric[1] + t.v2[2] * barycentric[2];

        if (!check_z_buffer(p)) continue;

        m_z_buffer[p.y*m_image.get_width() + p.x] = p.z;

        sk::vec3f n = cross((v2.pos - v0.pos), (v1.pos - v0.pos));
        sk::vec3f light_dir(0, 0, -1);
        n.norm();
        float intensity = dot(n, light_dir);

        TGAColor c;

        if (texture == nullptr)
        {
            c = get_point_color_from_vertices(barycentric, v0.color, v1.color, v2.color);
        }
        else
        {
            c = get_point_color_from_texture(barycentric, v0.tex, v1.tex, v2.tex, *texture);
        }

        m_image.set(p.x, p.y, TGAColor(c.r*intensity, c.g * intensity, c.b*intensity, c.a));
    }
}

bool sk::renderer::check_z_buffer(const sk::point& p)
{
    return m_z_buffer[p.y*m_image.get_width() + p.x] < p.z;
}

TGAColor sk::renderer::get_point_color_from_vertices(const sk::vec3f& barycentric,
    const TGAColor& v0_color, const TGAColor& v1_color, const TGAColor& v2_color)
{
    auto r = v0_color.r * barycentric[0] + v1_color.r * barycentric[1] + v2_color.r * barycentric[2];
    auto g = v0_color.g * barycentric[0] + v1_color.g * barycentric[1] + v2_color.g * barycentric[2];
    auto b = v0_color.b * barycentric[0] + v1_color.b * barycentric[1] + v2_color.b * barycentric[2];
    auto a = v0_color.a * barycentric[0] + v1_color.a * barycentric[1] + v2_color.a * barycentric[2];

    return TGAColor(r, g, b, a);
}

TGAColor sk::renderer::get_point_color_from_texture(const sk::vec3f& barycentric, const sk::vec2f& v0_tex_coord, const sk::vec2f& v1_tex_coord, const sk::vec2f& v2_tex_coord, const TGAImage& texture)
{
    auto u = v0_tex_coord[0] * barycentric[0] + v1_tex_coord[0] * barycentric[1] + v2_tex_coord[0] * barycentric[2];
    auto v = v0_tex_coord[1] * barycentric[0] + v1_tex_coord[1] * barycentric[1] + v2_tex_coord[1] * barycentric[2];

    auto u_image = static_cast<int>(u * texture.get_width() + 0.5);
    auto v_image = static_cast<int>(v * texture.get_width() + 0.5);

    return texture.get(u_image, v_image);
}

sk::vec3f sk::renderer::from_ndc(const sk::vec3f& v)
{
    sk::vec3f res;
    res[0] = static_cast<int>((v[0] + 1.) * m_image.get_width() / 2. + 0.5f);
    res[1] = static_cast<int>((v[1] + 1.) * m_image.get_height() / 2. + 0.5f);
    res[2] = v[2];

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
