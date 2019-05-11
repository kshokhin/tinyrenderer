#pragma once

#include"geometry.h"
#include"tgalib/tgaimage.h"

namespace sk
{
    class shader_base
    {
    public:
        virtual ~shader_base() = default;

        virtual vec4f vertex(const vec3f& pos) = 0;
        virtual bool fragment(const vec2f& uv, const vec3f& light, const vec3f& view, TGAColor& c) = 0;
    };

    class shader : public shader_base
    {
    public:
        virtual ~shader() = default;

        virtual vec4f vertex(const vec3f& pos) override;
        virtual bool fragment(const vec2f& uv, const vec3f& light, const vec3f& view, TGAColor& c) override;
        void set_transform_matrix(const matrix4x4f& m) { m_transform = m; }
        void set_texture(const TGAImage* tex) { m_texture = tex; }
        void set_normal_map(const TGAImage* nm) { m_normal_map = nm; }
        void set_specular_map(const TGAImage* sm) { m_specular_map = sm; }
    private:
        float calc_diff_intensity(const sk::vec3f& normal, const sk::vec3f& light);
        float calc_spec_intensity(int spec, const sk::vec3f& normal, const sk::vec3f& light, const vec3f& view);
        int phong_color(int base_color, float diff_intensity, float spec_intensity);
    private:
        matrix4x4f m_transform;
        const TGAImage *m_texture = nullptr;
        const TGAImage *m_normal_map = nullptr;
        const TGAImage *m_specular_map = nullptr;
        const float ambient_power = 5.f;
        const float specular_power = 1.4f;
        const float diffuse_power = 1.f;
    };
};