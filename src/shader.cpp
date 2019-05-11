#include"shader.h"

using namespace sk;

vec4f shader::vertex(const vec3f& pos)
{
    matrix1x4f pos_4d;
    pos_4d[0][0] = pos[0];
    pos_4d[0][1] = pos[1];
    pos_4d[0][2] = pos[2];
    pos_4d[0][3] = 1;

    /*std::cout << "===\n";
    std::cout << m_transform << "\n";
    std::cout << "++++\n";*/

    auto transformed = mul(pos_4d, m_transform);

    //std::cout << transformed << "\n";
    //std::cout << "===\n";

    return vec4f(transformed[0][0], transformed[0][1], transformed[0][2], transformed[0][3]);
}

bool shader::fragment(const vec2f& uv, const vec3f& light, const vec3f& view, TGAColor& c)
{
    auto u_image = static_cast<int>(uv[0] * m_texture->get_width() + 0.5);
    auto v_image = static_cast<int>(uv[1] * m_texture->get_height() + 0.5);

    auto base_color = m_texture->get(u_image, v_image);
    auto spec = m_specular_map->get(u_image, v_image).raw[0];
    auto n = m_normal_map->get(u_image, v_image);
    vec3f normal(n.r / 255.f*2.f - 1.f, n.g / 255.f*2.f - 1.f, n.b / 255.f*2.f - 1.f);
    //TODO: now assuming that transform matrix is identity matrix

    normal.norm();

    auto diff_intensity = calc_diff_intensity(normal, light);
    auto spec_intensity = diff_intensity > 0 ? calc_spec_intensity(spec, normal, light, view) : 0;

    c.r = phong_color(base_color.r, diff_intensity, spec_intensity);
    c.g = phong_color(base_color.g, diff_intensity, spec_intensity);
    c.b = phong_color(base_color.b, diff_intensity, spec_intensity);
    c.a = base_color.a;

    return false;
}

float shader::calc_diff_intensity(const sk::vec3f& normal, const sk::vec3f& light)
{
    auto intensity = dot(normal, light);

    if (intensity > 0) {
        intensity = 0;
    }
    else {
        intensity *= -1;
    }

    return intensity;
}

float shader::calc_spec_intensity(int spec, const sk::vec3f& normal, const sk::vec3f& light, const vec3f& view)
{
    auto r = normal*(dot(normal, light*(-1.f))*2.f) + light;
    r.norm();
    auto res = std::pow(std::max(dot(r, view*(-1.f)), 0.0f), spec);
    return (float)res;
}

int shader::phong_color(int base_color, float diff_intensity, float spec_intensity)
{
    //std::cout << "diff: " << diff_intensity << "\n";
    //std::cout << "spec: " << spec_intensity << "\n";
    return std::min(255, static_cast<int>(ambient_power + base_color*(diff_intensity*diffuse_power + spec_intensity*specular_power)));
}