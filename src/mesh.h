#pragma once
#include <vector>
#include <string>
#include <memory>
#include "geometry.h"
#include "tgalib/tgaimage.h"

struct model
{
    explicit model(const std::string& /*filename*/);

    std::vector<sk::vec3f> verts;
    std::vector<std::vector<int>> faces;
    std::unique_ptr<TGAImage> texture = nullptr;
private:
    void read_model(const std::string& /*filename*/);

};
