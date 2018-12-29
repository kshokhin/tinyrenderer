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
    draw_line_impl(l, true, c);
}

void sk::renderer::draw_line_impl(const sk::line& l, bool steep, const TGAColor& c)
{
    auto point = sk::point(l.start().x, l.start().y);
    auto err = 0;

    while(point.x != l.end().x)
    {
        l.steep() ? m_image.set(point.y, point.x, c) :
            m_image.set(point.x, point.y, c);

    point = get_next_line_point(l, point, err);
    }
}

void sk::renderer::draw(const model& m)
{
    for (const auto& f : m.faces)
    {
    const auto& v0 = m.verts[f[0]];
    const auto& v1 = m.verts[f[1]];
    const auto& v2 = m.verts[f[2]];
        draw_triangle(v0, v1, v2);
    }
}

void sk::renderer::draw_triangle(
        const sk::vec3f& v0,
        const sk::vec3f& v1,
        const sk::vec3f& v2)
{
    draw_line(v0, v1);
    draw_line(v1, v2);
    draw_line(v2, v0);
}

void sk::renderer::draw_line(const sk::vec3f& v0, const sk::vec3f& v1)
{
    TGAColor white(255, 255, 255, 255);
    int x0 = (v0[0] + 1.) * m_image.get_width() / 2.;
    int y0 = (v0[1] + 1.) * m_image.get_height() / 2.;
    int x1 = (v1[0] + 1.) * m_image.get_width() / 2.;
    int y1 = (v1[1] + 1.) * m_image.get_height() / 2.;
    draw_line(x0, y0, x1, y1, white); 
}


void sk::renderer::draw_filled_triangle(
        const sk::vec3f& v0,
        const sk::vec3f& v1,
        const sk::vec3f& v2,
        const TGAColor& c)
{
    auto v0_copy = v0;
    auto v1_copy = v1;
    auto v2_copy = v2;

    sort_left_to_right(v0_copy, v1_copy, v2_copy);
    
    std::cout << "v0: " << v0_copy[0] << "," << v0_copy[1] << "," << v0_copy[2] << "\n"; 
    std::cout << "v1: " << v1_copy[0] << "," << v1_copy[1] << "," << v1_copy[2] << "\n"; 
    std::cout << "v2: " << v2_copy[0] << "," << v2_copy[1] << "," << v2_copy[2] << "\n"; 

    int x0 = (v0_copy[0] + 1.) * m_image.get_width() / 2.;
    int y0 = (v0_copy[1] + 1.) * m_image.get_height() / 2.;
    int x1 = (v1_copy[0] + 1.) * m_image.get_width() / 2.;
    int y1 = (v1_copy[1] + 1.) * m_image.get_height() / 2.;
    int x2 = (v2_copy[0] + 1.) * m_image.get_width() / 2.;
    int y2 = (v2_copy[1] + 1.) * m_image.get_height() / 2.;

    auto line1 = sk::line(x0, y0, x1, y1);
    auto line2 = sk::line(x0, y0, x2, y2);
    auto line3 = sk::line(x1, y1, x2, y2);

    //draw(line1, c);
    //draw(line2, c);
    //draw(line3, c);
    //draw_filled_triangle(line1, line2, line3);
    auto point1 = line1.start(), point2 = line2.start();

    int err1 = 0, err2 = 0;
    while (point1.x != line1.end().x && point2.x != line2.end().x)
    {
        auto x1 = line1.steep() ? point1.y : point1.x;
        auto x2 = line2.steep() ? point2.y : point2.x;
        auto y1 = line1.steep() ? point1.x : point1.y;
        auto y2 = line2.steep() ? point2.x : point2.y;

        draw_line(x1, y1, x2, y2, c);
        point1 = get_next_line_point(line1, point1, err1);
        point2 = get_next_line_point(line2, point2, err2);
    }
}

void sk::renderer::draw_filled_triangle(const sk::line& border1, const sk::line& border2, const sk::line& border3)
{

}

void sk::renderer::sort_left_to_right(sk::vec3f& v0, sk::vec3f& v1, sk::vec3f& v2)
{
    if (v0[0] > v1[0])
    {
        std::swap(v0, v1);
    }

    if (v0[0] > v2[0])
    {
        std::swap(v0, v2);
    }
    
    if (v1[0] > v2[0])
    {
        std::swap(v1, v2);
    }
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
