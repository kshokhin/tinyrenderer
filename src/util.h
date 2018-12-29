#pragma once

#include "geometry.h"

namespace sk
{
struct point
{
    point() = default;
    point(int _x, int _y) : x(_x), y(_y) {}
    
    bool operator==(const point& rhs) const
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

class triangle
{
public:
    triangle() = default;
    triangle(const sk::vec3i& _v0, const sk::vec3i& _v1, const sk::vec3i& _v2) : v0(_v0), v1(_v1), v2(_v2) {};

    bool contains(const point&);

    vec3i v0;
    vec3i v1;
    vec3i v2;
private:
    vec3f barycentric(const point&);
};

class bounding_box
{
public:
    class bounding_box_iterator
    {
    public:
        explicit bounding_box_iterator(const bounding_box&, bool /*end_reached*/ = false);

        bounding_box_iterator& operator++();
        point& operator*() { return m_point; }
        bool operator==(const bounding_box_iterator& rhs) const { return m_end_reached == rhs.m_end_reached && (m_end_reached || m_point == rhs.m_point); }
        bool operator!=(const bounding_box_iterator& rhs) const { return !(*this == rhs); }
    private:
        const bounding_box& m_owner;
        point m_point;
        bool m_end_reached = false;
    };

    class bounding_box_const_iterator
    {
    public:
        explicit bounding_box_const_iterator(const bounding_box&, bool /*end_reached*/ = false);

        const bounding_box_const_iterator& operator++();
        const point& operator*() const { return m_point; }
        bool operator==(const bounding_box_const_iterator& rhs) const { return m_end_reached == rhs.m_end_reached && (m_end_reached || m_point == rhs.m_point); }
        bool operator!=(const bounding_box_const_iterator& rhs) const { return !(*this == rhs); }
    private:
        const bounding_box& m_owner;
        point m_point;
        bool m_end_reached = false;
    };


    explicit bounding_box(const triangle&);

    bounding_box_iterator begin();
    bounding_box_iterator end();

    bounding_box_const_iterator begin() const;
    bounding_box_const_iterator end() const;

    point first_point() const;
    point next_point(const point&) const;

private:
    vec3i m_min;
    vec3i m_max;
};
}
