#include "mesh.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

model::model(const std::string& filename, const std::string& texture_filename)
{
    read_model(filename);
    read_texture(texture_filename);
}

void model::read_model(const std::string& filename)
{
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            sk::vec3f v;
            for (size_t i = 0; i < 3; ++i) iss >> v[i];
            verts.push_back(v);
        }
        else if (!line.compare(0, 3, "vn "))
        {
            iss >> trash >> trash;
            sk::vec3f n;
            for (size_t i = 0; i < 3; ++i) iss >> n[i];
            n.norm();
            normals.push_back(n);
        }
        else if (!line.compare(0, 2, "f "))
        {
            face f;
            int idx, tex_idx, n_idx;
            iss >> trash;
            while (iss >> idx >> trash >> tex_idx >> trash >> n_idx) {
                idx--; // in wavefront obj all indices start at 1, not zero
                tex_idx--;
                n_idx--;
                f.vert_ids.push_back(idx);
                f.tex_ids.push_back(tex_idx);
                f.norm_ids.push_back(n_idx);
            }
            faces.push_back(f);
        }
        else if (!line.compare(0, 3, "vt "))
        {
            sk::vec2f tv;
            iss >> trash >> trash;
            for (size_t i = 0; i < 2; ++i) iss >> tv[i];
            tex_verts.push_back(tv);
        }
    }
    std::cerr << "# v# " << verts.size() << " f# "  << faces.size() << "\n";

    if (normals.empty())
    {
        calc_vertex_normals();
    }
}

void model::read_texture(const std::string& filename)
{
    if (filename.empty()) return;

    try
    {
        texture = std::make_unique<TGAImage>();
    }
    catch (std::bad_alloc&)
    {
        std::cerr << "out of memory\n";
        return;
    }

    if (!texture->read_tga_file(filename.c_str()))
    {
        texture.reset();
    }

    texture->flip_vertically();
}

sk::vertex model::get_vertex(const face& f, size_t vertex_id)
{
    sk::vertex v;

    v.pos = verts[f.vert_ids[vertex_id]];
    if (texture != nullptr)
    {
        v.tex[0] = tex_verts[f.tex_ids[vertex_id]][0];
        v.tex[1] = tex_verts[f.tex_ids[vertex_id]][1];
    }
    v.normal = normals[f.norm_ids[vertex_id]];
    v.color = get_vertex_color(f, vertex_id);

    return v;
}

TGAColor model::get_vertex_color(const face& f, size_t vertex_id)
{
    return texture == nullptr ? TGAColor(255, 255, 255, 255) : 
        texture->get(tex_verts[f.tex_ids[vertex_id]][0], tex_verts[f.tex_ids[vertex_id]][1]);
}

void model::calc_vertex_normals()
{
    for (size_t v_idx = 0; v_idx < verts.size(); ++v_idx)
    {
        calc_vertex_normal(v_idx);
    }
}

void model::calc_vertex_normal(size_t v_idx)
{
    auto faces = find_vertex_faces(v_idx);
    auto faces_normals = calc_faces_normals(faces);

    auto v_norm = sk::average(faces_normals);
    v_norm.norm();

    normals.push_back(v_norm);
    attach_normal_to_faces(faces, normals.size() - 1, v_idx);
}

std::vector<size_t> model::find_vertex_faces(size_t v_idx)
{
    std::vector<size_t> incident_faces;

    for (size_t i = 0; i < faces.size(); ++i)
    {
        for (auto v : faces[i].vert_ids)
        {
            if (v != v_idx) continue;

            incident_faces.push_back(i);
        }
    }

    return incident_faces;
}

std::vector<sk::vec3f> model::calc_faces_normals(std::vector<size_t> face_indexes)
{
    std::vector<sk::vec3f> face_normals;
    for (auto f : face_indexes)
    {
        sk::vec3f n = calc_face_normal(f); 
        face_normals.push_back(n);
    }

    return face_normals;
}

sk::vec3f model::calc_face_normal(size_t face_idx)
{
    auto& v0_pos = verts[faces[face_idx].vert_ids[0]];
    auto& v1_pos = verts[faces[face_idx].vert_ids[1]];
    auto& v2_pos = verts[faces[face_idx].vert_ids[2]];

    return sk::cross((v1_pos - v0_pos), (v2_pos - v0_pos));
}

void model::attach_normal_to_faces(std::vector<size_t> face_indexes, size_t n_idx, size_t v_idx)
{
    for (auto f_idx : face_indexes)
    {
        attach_normal_to_face(faces[f_idx], n_idx, v_idx);
    }
}

void model::attach_normal_to_face(face& f, size_t n_idx, size_t v_idx)
{
    f.norm_ids.resize(f.vert_ids.size());
    for (size_t i = 0; i < f.vert_ids.size(); ++i)
    {
        if (f.vert_ids[i] != v_idx) continue;

        f.norm_ids[i] = n_idx;
    }
}
