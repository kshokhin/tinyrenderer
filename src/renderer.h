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
    void draw_filled_triangle(const sk::vertex& /*v0*/, const sk::vertex& /*v1*/, const sk::vertex& /*v2*/, TGAImage* /*texture*/ = nullptr); 
    void look_at(const sk::vec3f& eye, const sk::vec3f& center, const sk::vec3f& up);
    void set_world_matrix(const matrix4x4f& m);
    void set_view_matrix(const matrix4x4f& m);
    void set_projection_matrix(const matrix4x4f& m);
    void set_fov(float fov_rads);
private:
    sk::point get_next_line_point(const sk::line&, const point& /*prev_point*/, int& /*err*/);
    sk::vec3f from_ndc(const sk::vec3f& /*v*/);
    bool check_z_buffer(const sk::point&);
    TGAColor get_point_color_from_vertices(const sk::vec3f& /*barycentric*/,
        const TGAColor& /*v0_color*/, const TGAColor& /*v1_color*/, const TGAColor& /*v2_color*/);
    TGAColor get_point_color_from_texture(const sk::vec3f& /*barycentric*/, const sk::vec2f& /*v0_tex_coord*/, const sk::vec2f& /*v1_tex_coord*/, const sk::vec2f& /*v2_tex_coord*/, const TGAImage& texture);
    sk::vec3f transform_vertex(const sk::vec3f&);
    void init_transformation_pipeline();
    void draw(sk::triangle& t, const sk::vec3f& intensity);
    sk::vec3f triangle_intensity(const sk::vertex& v0, const sk::vertex& v1, const sk::vertex& v2);
    bool backface_cull(const sk::vertex& v0, const sk::vertex& v1, const sk::vertex& v2);
    float vertex_intensity(const sk::vertex& v, const sk::vec3f& light_dir);
private:
    TGAImage& m_image;
    std::vector<float> m_z_buffer;
    sk::vec3f m_view;
    sk::matrix4x4f m_world_matrix;
    sk::matrix4x4f m_view_matrix;
    sk::matrix4x4f m_projection_matrix;
};

}
