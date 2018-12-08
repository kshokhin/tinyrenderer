#pragma once
#include <vector>
#include <string>
#include "geometry.h"

struct model
{
    explicit model(const std::string& /*filename*/);

    std::vector<sk::vec3f> verts;
    std::vector<std::vector<int>> faces;
private:
    void read_model(const std::string& /*filename*/);

};
