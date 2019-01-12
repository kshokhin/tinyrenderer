#include "tgalib/tgaimage.h"
#include "renderer.h"
#include "mesh.h"

#define WIDTH 800
#define HEIGHT 800

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

int main(int argc, char** argv) {
    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    model m("african_head.obj");
    sk::renderer r(image);

    sk::vec3f light_dir(0, 0, -1);

    for (auto& face : m.faces) {
        auto& v0 = m.verts[face[0]];
        auto& v1 = m.verts[face[1]];
        auto& v2 = m.verts[face[2]];
        sk::vec3f n = cross((v2 - v0), (v1 - v0));
        n.norm();
        float intensity = dot(n, light_dir);
//        if (intensity > 0)
//        {
        r.draw_filled_triangle(v0, v1, v2, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
//        }
    }

    return 0;
}

