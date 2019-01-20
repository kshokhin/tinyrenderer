#include"util.h"
#include<cmath>
#include<cstdlib>
#include<utility>

#include"tgalib/tgaimage.h"

using namespace sk;

line::line(int x0, int y0, int x1, int y1) : m_start(x0, y0), m_end(x1, y1)
{
    prepare();
}

line::line(const point& s, const point& e) : m_start(s), m_end(e)
{
    prepare();
}

void line::prepare()
{
    m_steep = std::abs(m_start.x - m_end.x) < std::abs(m_start.y - m_end.y);

    if (m_steep)
    {
        std::swap(m_start.x, m_start.y);
        std::swap(m_end.x, m_end.y);
    }
    
    if (m_start.x > m_end.x)
    {
        std::swap(m_start, m_end);
    }

    m_dx = m_end.x - m_start.x;
    m_dy = m_end.y - m_start.y;
    m_derr = 2 * std::abs(m_dy);
}

bool triangle::contains(const point& p)
{
    vec3f barycentric_coords = barycentric(p);

//    std::cout << barycentric_coords[0] << " " << barycentric_coords[1] << " " << barycentric_coords[2] << "\n";

    return barycentric_coords[0] >= 0 && barycentric_coords[1] >= 0 && barycentric_coords[2] >= 0;
}

vec3f triangle::barycentric(const point& p) const
{
    vec3f s0, s1;
    s0[0] = v2[0] - v0[0];
    s0[1] = v1[0] - v0[0];
    s0[2] = v0[0] - p.x;

    s1[0] = v2[1] - v0[1];
    s1[1] = v1[1] - v0[1];
    s1[2] = v0[1] - p.y;

    //std::cout << v0 << v1 << v2 << p.x << " " << p.y << "\n";

    vec3f u = cross(s0, s1);

    if (std::abs(u[2]) > 1e-2) //u[2] is integer. If it is zero then triangle ABC is degenerate
        return vec3f(1.f - (u[0] + u[1]) / u[2], u[1] / u[2], u[0] / u[2]);
    return vec3f(-1, 1, 1);

}

bounding_box::bounding_box_iterator::bounding_box_iterator(const bounding_box& bb, bool end_reached) : m_owner(bb), m_end_reached(end_reached)
{
    if (!end_reached)
    {
        m_point = m_owner.first_point();
    }
}

bounding_box::bounding_box_iterator& bounding_box::bounding_box_iterator::operator++()
{
    if (!m_end_reached)
    {
        auto prev = m_point;
        m_point = m_owner.next_point(m_point);
        m_end_reached = m_point == prev;
    }

    return *this;
}

bounding_box::bounding_box_const_iterator::bounding_box_const_iterator(const bounding_box& bb, bool end_reached) : m_owner(bb), m_end_reached(end_reached)
{
    if (!end_reached)
    {
        m_point = m_owner.first_point();
    }
}

const bounding_box::bounding_box_const_iterator& bounding_box::bounding_box_const_iterator::operator++()
{
    if (!m_end_reached)
    {
        auto prev = m_point;
        m_point = m_owner.next_point(m_point);
        m_end_reached = m_point == prev;
    }

    return *this;
}

bounding_box::bounding_box(const triangle& t, const TGAImage& i)
{
    m_min[0] = std::min(t.v0[0], std::min(t.v1[0], t.v2[0]));
    m_min[1] = std::min(t.v0[1], std::min(t.v1[1], t.v2[1]));

    m_max[0] = std::max(t.v0[0], std::max(t.v1[0], t.v2[0]));
    m_max[1] = std::max(t.v0[1], std::max(t.v1[1], t.v2[1]));

    m_max[0] = std::min(i.get_width() - 1, m_max[0]);
    m_max[1] = std::min(i.get_height() - 1, m_max[1]);
}

bounding_box::bounding_box_iterator bounding_box::begin()
{
    return bounding_box_iterator(*this, false);
}

bounding_box::bounding_box_iterator bounding_box::end()
{
    return bounding_box_iterator(*this, true);
}

bounding_box::bounding_box_const_iterator bounding_box::begin() const
{
    return bounding_box_const_iterator(*this, false);
}

bounding_box::bounding_box_const_iterator bounding_box::end() const
{
    return bounding_box_const_iterator(*this, true);
}

point bounding_box::first_point() const
{
    return point(m_min[0], m_min[1]);
}

point bounding_box::next_point(const point& prev_point) const
{
    auto x = prev_point.x;
    auto y = prev_point.y + 1;

    //if y is out of bounds switch to next row
    if (y > m_max[1])
    {
        y = m_min[1];
        x++;
    }

    //if x is out of bounds return to prev point
    if (x > m_max[0])
    {
        x = m_max[0];
        y = m_max[1];
    }

    return point(x, y);
}
