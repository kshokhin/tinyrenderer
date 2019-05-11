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

bool shader::fragment(const vec2f& uv, const vec3f& light, TGAColor& c)
{
    auto u_image = static_cast<int>(uv[0] * m_texture->get_width() + 0.5);
    auto v_image = static_cast<int>(uv[1] * m_texture->get_height() + 0.5);

    auto base_color = m_texture->get(u_image, v_image);
    auto n = m_normal_map->get(u_image, v_image);
    vec3f n_c(n.r / 255.f*2.f - 1.f, n.g / 255.f*2.f - 1.f, n.b / 255.f*2.f - 1.f);
    //std::cout << n_c << "\n";
    //TODO: now assuming that transform matrix is identity matrix

    n_c.norm();

    auto intensity = dot(n_c, light);

    if (intensity > 0) {
        intensity = 0;
    }
    else {
        intensity *= -1;
    }

    c.r = base_color.r * intensity;
    c.g = base_color.g * intensity;
    c.b = base_color.b * intensity;
    c.a = base_color.a;

    return false;
}