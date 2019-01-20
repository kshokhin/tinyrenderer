#include "tgalib/tgaimage.h"
#include "renderer.h"
#include "mesh.h"

#define WIDTH 800
#define HEIGHT 800

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

int main(int argc, char** argv) {
    TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    model m("african_head.obj");
    sk::renderer r(image);

    sk::vec3f light_dir(0, 0, -1);

    for (auto& face : m.faces) {
        sk::vertex v0, v1, v2;
        v0.pos = m.verts[face[0]];
        v0.color = white;

        v1.pos = m.verts[face[1]];
        v1.color = white;

        v2.pos = m.verts[face[2]];
        v2.color = white;

        r.draw_filled_triangle(v0, v1, v2);
    }

    

   /* v0.pos = sk::vec3f(0.f, 0.5f, 0);
    v0.color = green;

    v1.pos = sk::vec3f(-0.5f, -0.5f, 0);
    v1.color = red;

    v2.pos = sk::vec3f(0.5f, -0.5f, 0);
    v2.color = blue;

    r.draw_filled_triangle(v0, v1, v2);*/

    return 0;
}

