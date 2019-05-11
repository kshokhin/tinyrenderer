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
        virtual bool fragment(const vec2f& uv, const vec3f& light, TGAColor& c) = 0;
    };

    class shader : public shader_base
    {
    public:
        virtual ~shader() = default;

        virtual vec4f vertex(const vec3f& pos) override;
        virtual bool fragment(const vec2f& uv, const vec3f& light, TGAColor& c) override;
        void set_transform_matrix(const matrix4x4f& m) { m_transform = m; }
        void set_texture(const TGAImage* tex) { m_texture = tex; }
        void set_normal_map(const TGAImage* nm) { m_normal_map = nm; }
    private:
        matrix4x4f m_transform;
        const TGAImage *m_texture = nullptr;
        const TGAImage *m_normal_map = nullptr;
    };
};