#include"util.h"
#include<cmath>
#include<cstdlib>
#include<utility>

sk::line::line(int x0, int y0, int x1, int y1) : m_start(x0, y0), m_end(x1, y1)
{
    prepare();
}

sk::line::line(const point& s, const point& e) : m_start(s), m_end(e)
{
    prepare();
}

void sk::line::prepare()
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
