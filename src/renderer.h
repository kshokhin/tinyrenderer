#pragma once
#include <iostream>
#include <vector>

#include"tgalib/tgaimage.h"
#include"util.h"

struct model;

namespace sk
{

template<typename T, size_t ...indexes>
class vec_impl;

using vec3f = vec_impl<float, 0, 1, 2>;

class renderer final
{
public:
    explicit renderer(TGAImage& target);
    renderer(const renderer&) = delete;
    renderer(renderer&&) = delete;
    renderer& operator=(const renderer&) = delete;
    renderer& operator=(renderer&&) = delete; 
    ~renderer();
     
    void draw(const sk::point&, const TGAColor&);
    void draw(const sk::line&, const TGAColor&);
    void draw_wireframe(const model&, const TGAColor&);
    void draw_line(int /*x0*/,int /*y0*/,int /*x1*/,int /*y1*/, const TGAColor&);
    void draw_line(const sk::vec3f& /*v0*/, const sk::vec3f& /*v1*/, const TGAColor&);
    void draw_triangle(const sk::vec3f& /*v0*/, const sk::vec3f& /*v1*/, const sk::vec3f& /*v2*/, const TGAColor&);
    void draw_filled_triangle(const sk::vec3f& /*v0*/, const sk::vec3f& /*v1*/, const sk::vec3f& /*v2*/, const TGAColor&); 
private:
    sk::point get_next_line_point(const sk::line&, const point& /*prev_point*/, int& /*err*/);
    sk::vec3f from_ndc(const sk::vec3f& /*v*/);
    bool check_z_buffer(const sk::point&);
    float calc_z_coordinate(const sk::point&, const sk::triangle&);
private:
    TGAImage& m_image;
    std::vector<float> m_z_buffer;
};

}
