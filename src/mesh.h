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
    std::vector<int> norm_ids;
};

struct model
{
    explicit model(const std::string& /*filename*/, const std::string& /*texture_filename*/,
        const std::string& /*normal_map_filename*/, const std::string& /*spec_filename*/);

    sk::vertex get_vertex(const face&, size_t /*vertex_id*/);

    std::vector<sk::vec3f> verts;
    std::vector<face> faces;
    std::vector<sk::vec2f> tex_verts;
    std::vector<sk::vec3f> normals;
    std::unique_ptr<TGAImage> texture = nullptr;
    std::unique_ptr<TGAImage> normal_map = nullptr;
    std::unique_ptr<TGAImage> specular_map = nullptr;
private:
    void read_model(const std::string& /*filename*/);
    void read_texture(std::unique_ptr<TGAImage>& tex, const std::string& /*filename*/);
    TGAColor get_vertex_color(const face&, size_t /*vertex_id*/);
    void calc_vertex_normals();
    void calc_vertex_normal(size_t v_idx);
    std::vector<size_t> find_vertex_faces(size_t v_idx);
    std::vector<sk::vec3f> calc_faces_normals(std::vector<size_t> face_indexes);
    sk::vec3f calc_face_normal(size_t face_idx);
    void attach_normal_to_faces(std::vector<size_t> face_indexes, size_t n_idx, size_t v_idx);
    void attach_normal_to_face(face& f, size_t n_idx, size_t v_idx);
};
