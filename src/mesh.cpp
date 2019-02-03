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
        else if (!line.compare(0, 2, "f "))
        {
            face f;
            int itrash, idx, tex_idx;
            iss >> trash;
            while (iss >> idx >> trash >> tex_idx >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                tex_idx--;
                f.vert_ids.push_back(idx);
                f.tex_ids.push_back(tex_idx);
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
    v.color = get_vertex_color(f, vertex_id);

    return v;
}

TGAColor model::get_vertex_color(const face& f, size_t vertex_id)
{
    return texture == nullptr ? TGAColor(255, 255, 255, 255) : 
        texture->get(tex_verts[f.tex_ids[vertex_id]][0], tex_verts[f.tex_ids[vertex_id]][1]);
}
