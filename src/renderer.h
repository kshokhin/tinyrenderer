#pragma once
#include <iostream>

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
    void draw(const model&);
    void draw_line(int /*x0*/,int /*y0*/,int /*x1*/,int /*y1*/, const TGAColor&);
    void draw_line(const sk::vec3f& /*v0*/, const sk::vec3f& /*v1*/);
    void draw_triangle(const sk::vec3f& /*v0*/, const sk::vec3f& /*v1*/, const sk::vec3f& /*v2*/);
 
private:
    void draw_line_impl(const sk::line&, bool /*steep*/, const TGAColor&);
private:
    TGAImage& m_image;
};

}
