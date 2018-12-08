#include "mesh.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

model::model(const std::string& filename)
{
    read_model(filename);
}

/*size_t model::verts_num()
{
    return m_verts.size();
}

size_t model::faces_num()
{
    return m_faces.size();
}

std::vector<int> model::face(size_t idx)
{
    return m_faces[idx];
}

sk::vec3f model::vert(size_t idx)
{
    return m_verts[idx];
}*/

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
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
	    sk::vec3f v;
            for (size_t i=0;i<3;i++) iss >> v[i];
            verts.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
            }
            faces.push_back(f);
        }
    }
    std::cerr << "# v# " << verts.size() << " f# "  << faces.size() << "\n";
}
