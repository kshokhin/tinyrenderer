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
    /*TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
    model m("african_head.obj", "african_head_diffuse.tga");
    sk::renderer r(image);

    r.set_camera_pos(3.f);

    sk::vec3f light_dir(0, 0, -1);

    for (auto& face : m.faces) {
        sk::vertex v0, v1, v2;

        v0 = m.get_vertex(face, 0);
        v1 = m.get_vertex(face, 1);
        v2 = m.get_vertex(face, 2);

        r.draw_filled_triangle(v0, v1, v2, m.texture.get());
    }*/

    

   /* v0.pos = sk::vec3f(0.f, 0.5f, 0);
    v0.color = green;

    v1.pos = sk::vec3f(-0.5f, -0.5f, 0);
    v1.color = red;

    v2.pos = sk::vec3f(0.5f, -0.5f, 0);
    v2.color = blue;

    r.draw_filled_triangle(v0, v1, v2);*/

    sk::vec3f v(1.f, 2.f, 3.f);
    sk::matrix3x3f m1, m2;

    std::vector<float> row{ 1.f, 2.f, 3.f };

    m1.set_row(row, 0);
    m1.set_row(row, 1);
    m1.set_row(row, 2);

    std::cout << m1 << "\n";

    m2.set_row(row, 0);
    m2.set_row(row, 1);
    m2.set_row(row, 2);

    std::cout << m2 << "\n";

    auto m1_x_m2 = mul(m1, m2);
    auto m1_x_v = mul(m1, static_cast<sk::matrix3x1f>(v));

    std::cout << m1_x_m2 << "\n";
    std::cout << m1_x_v << "\n";

    return 0;
}

