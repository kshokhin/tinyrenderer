#include "renderer.h"
#include "mesh.h"
#include "shader.h"

#include <utility>
#include <limits>

sk::renderer::renderer(TGAImage& target) : m_image(target), m_z_buffer(m_image.get_height() * m_image.get_width(), -std::numeric_limits<float>::max()) { init_transformation_pipeline(); }

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
        const auto& v0 = m.verts[f.vert_ids[0]];
        const auto& v1 = m.verts[f.vert_ids[1]];
        const auto& v2 = m.verts[f.vert_ids[2]];
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

    auto intensity = triangle_intensity(v0, v1, v2);
    if (intensity[0] < 0 || intensity[1] < 0 || intensity[2] < 0) return;

    auto v0_transformed = transform_vertex(v0.pos);
    auto v1_transformed = transform_vertex(v1.pos);
    auto v2_transformed = transform_vertex(v2.pos);

    sk::triangle t(from_ndc(v0_transformed), from_ndc(v1_transformed), from_ndc(v2_transformed));
    if (texture != nullptr)
    {
        t.set_texture(*texture, v0.tex, v1.tex, v2.tex);
    }
    else
    {
        t.set_color(v0.color, v1.color, v2.color);
    }

    draw(t, intensity);
}

//negative intensity means that triangle is not supposed to be rendered
sk::vec3f sk::renderer::triangle_intensity(const sk::vertex& v0, const sk::vertex& v1, const sk::vertex& v2)
{
    sk::vec3f intensity{ -1.f, -1.f, -1.f };

    if (backface_cull(v0, v1, v2)) return intensity;

    sk::vec3f light_dir(-1, 0, 0);//world space coords
    light_dir.norm();

    intensity[0] = vertex_intensity(v0, light_dir);
    intensity[1] = vertex_intensity(v1, light_dir);
    intensity[2] = vertex_intensity(v2, light_dir);

    return intensity;
}

bool sk::renderer::backface_cull(const sk::vertex& v0, const sk::vertex& v1, const sk::vertex& v2)
{
    sk::vec3f n = cross((v1.pos - v0.pos), (v2.pos - v0.pos));
    n.norm();

    float angle = dot(n, m_view);

    return angle > 0;
}

float sk::renderer::vertex_intensity(const sk::vertex& v, const sk::vec3f& light_dir)
{
    float intensity = dot(v.normal, light_dir);
    if (intensity > 0) {
        intensity = 0;
    }
    else {
        intensity *= -1;
    }

    return intensity;
}

void sk::renderer::draw(sk::triangle& t, const sk::vec3f& intensity)
{
    sk::bounding_box bb(t, m_image);

    for (auto p : bb)
    {
        if (p.x < 0 || p.x > m_image.get_width() ||
            p.y < 0 || p.y > m_image.get_height()) continue;
        auto barycentric = t.barycentric(p);

        if (barycentric[0] < 0 || barycentric[1] < 0 || barycentric[2] < 0) continue;

        p.z = t.v0[2] * barycentric[0] + t.v1[2] * barycentric[1] + t.v2[2] * barycentric[2];

        if (!check_z_buffer(p)) continue;

        m_z_buffer[p.y*m_image.get_width() + p.x] = p.z;

        TGAColor c;
        sk::vec2f uv;
        uv[0] = t.v0_tex[0] * barycentric[0] + t.v1_tex[0] * barycentric[1] + t.v2_tex[0] * barycentric[2];
        uv[1] = t.v0_tex[1] * barycentric[0] + t.v1_tex[1] * barycentric[1] + t.v2_tex[1] * barycentric[2];

        sk::vec3f light_dir(0, 0, -1);//world space coords
        light_dir.norm();
        m_shader->fragment(uv, light_dir, m_view, c);

        m_image.set(p.x, p.y, c);

        /*TGAColor c;

        if (t.texture == nullptr)
        {
            c = get_point_color_from_vertices(barycentric, t.v0_col, t.v1_col, t.v2_col);
        }
        else
        {
            c = get_point_color_from_texture(barycentric, t.v0_tex, t.v1_tex, t.v2_tex, *t.texture);
        }

        auto point_intensity = intensity[0] * barycentric[0] + intensity[1] * barycentric[1] + intensity[2] * barycentric[2];
        m_image.set(p.x, p.y, TGAColor(c.r*point_intensity, c.g*point_intensity, c.b*point_intensity, c.a));*/
    }
}

void sk::renderer::look_at(const sk::vec3f& eye, const sk::vec3f& center, const sk::vec3f& up)
{
    auto camera_look = eye - center;
    camera_look.norm();
    m_view = center - eye;
    m_view.norm();
    std::cout << "view: " << m_view << "\n";
    auto camera_right = sk::cross(camera_look, up);
    camera_right.norm();
    auto camera_up = sk::cross(camera_look, camera_right);
    camera_up.norm();

    m_view_matrix[0][0] = camera_right[0];
    m_view_matrix[1][0] = camera_right[1];
    m_view_matrix[2][0] = camera_right[2];

    m_view_matrix[0][1] = camera_up[0];
    m_view_matrix[1][1] = camera_up[1];
    m_view_matrix[2][1] = camera_up[2];

    m_view_matrix[0][2] = camera_look[0];
    m_view_matrix[1][2] = camera_look[1];
    m_view_matrix[2][2] = camera_look[2];

    m_view_matrix[0][3] = 0;
    m_view_matrix[1][3] = 0;
    m_view_matrix[2][3] = 0;

    m_view_matrix[3][0] = -sk::dot(eye, camera_right);
    m_view_matrix[3][1] = -sk::dot(eye, camera_up);
    m_view_matrix[3][2] = -sk::dot(eye, camera_look);
    m_view_matrix[3][3] = 1;
}

void sk::renderer::set_world_matrix(const matrix4x4f& m)
{
    m_world_matrix = m;
}

void sk::renderer::set_view_matrix(const matrix4x4f& m)
{
    m_view_matrix = m;
}

void sk::renderer::set_projection_matrix(const matrix4x4f& m)
{
    m_projection_matrix = m;
}

void sk::renderer::set_fov(float fov_rads)
{
    auto cotan = 1.f / std::tan(fov_rads / 2);
    m_projection_matrix[0][0] = cotan;
    m_projection_matrix[1][1] = cotan;
}

sk::vec3f sk::renderer::transform_vertex(const sk::vec3f& pos)
{
    sk::vec3f res;
    auto transformed = m_shader->vertex(pos);

    res[0] = transformed[0];
    res[1] = transformed[1];
    res[2] = transformed[2];

    if (std::abs(transformed[3]) < 1e-5)
    {
        res[0] = -2;
        res[1] = -2;
    }
    else
    {
        res[0] /= transformed[3];
        res[1] /= transformed[3];
    }

    return res;
}

void sk::renderer::init_transformation_pipeline()
{
    using projection_type = decltype(m_projection_matrix);
    using view_type = decltype(m_view_matrix);
    using world_type = decltype(m_world_matrix);

    m_projection_matrix = sk::identity_matrix<projection_type::value_type, projection_type::row_cnt>();
    m_projection_matrix[3][3] = 0.f;
    m_projection_matrix[2][3] = 1.f;
    m_view_matrix = sk::identity_matrix<view_type::value_type, view_type::row_cnt>();
    m_world_matrix = sk::identity_matrix<world_type::value_type, world_type::row_cnt>();
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
    auto v_image = static_cast<int>(v * texture.get_height() + 0.5);

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