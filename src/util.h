#pragma once

namespace sk
{
struct point
{
    point() = default;
    point(int _x, int _y) : x(_x), y(_y) {}
    
    bool operator==(const point& rhs)
    {
        return x == rhs.x && y == rhs.y;
    }
    
    int x = 0;
    int y = 0;
};


class line
{
public:
    line() = default;
    line(int x0, int y0, int x1, int y1);
    line(const point& s, const point& e);

    const point& start() const { return m_start; }
    const point& end() const { return m_end; }
    int dx() const { return m_dx; }
    int dy() const { return m_dy; }
    long derr() const { return m_derr; }
    bool steep() const { return m_steep; }
private:
    void prepare();
private:
    point m_start;
    point m_end;
    int m_dx = 0;
    int m_dy = 0;
    bool m_steep = false;
    long m_derr = 0;

};
}
