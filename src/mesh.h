#pragma once
#include <vector>
#include <string>
#include <memory>
#include "geometry.h"
#include "util.h"
#include "tgalib/tgaimage.h"

struct face
{
    std::vector<int> vert_ids;
    std::vector<int> tex_ids;
};

struct model
{
    explicit model(const std::string& /*filename*/, const std::string& /*texture_filename*/ = "");

    sk::vertex get_vertex(const face&, size_t /*vertex_id*/);

    std::vector<sk::vec3f> verts;
    std::vector<face> faces;
    std::vector<sk::vec2f> tex_verts;
    std::vector<sk::vec3f> normals;
    std::unique_ptr<TGAImage> texture = nullptr;
private:
    void read_model(const std::string& /*filename*/);
    void read_texture(const std::string& /*filename*/);
    TGAColor get_vertex_color(const face&, size_t /*vertex_id*/);
};
