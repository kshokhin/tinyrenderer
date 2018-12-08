#include"renderer.h"
#include"mesh.h"

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
    sk::line l1 = l;
    
    bool steep = std::abs(l1.start.x - l1.end.x) < std::abs(l1.start.y - l1.end.y);

    if (steep)
    {
        std::swap(l1.start.x, l1.start.y);
	std::swap(l1.end.x, l1.end.y);
    }
    
    if (l1.start.x > l1.end.x)
    {
        std::swap(l1.start, l1.end);
    }

    draw_line_impl(l1, steep, c);
}

void sk::renderer::draw_line_impl(const sk::line& l, bool steep, const TGAColor& c)
{
    auto y = l.start.y, dx = l.end.x - l.start.x, dy = l.end.y - l.start.y;
    auto derr = 2 * std::abs(dy), err = 0;

    for (auto x = l.start.x; x <= l.end.x; ++x)
    {
        steep ? m_image.set(y, x, c) : m_image.set(x, y, c);

	err += derr;
	if (err >= dx)
	{
	     y += dy > 0 ? 1 : -1;
	     err -= 2 * dx;
	}
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
