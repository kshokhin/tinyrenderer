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
    /*sk::vec3f t0[3] = {sk::vec3f(-0.975, -0.825, 0),
	           sk::vec3f(-0.875, -0.6, 0),
		   sk::vec3f(-0.825, -0.8, 0)}; 
    sk::vec3f t1[3] = {sk::vec3f(-0.55, -0.875, 0),
	           sk::vec3f(-0.625, -1, 0),
		   sk::vec3f(-0.825, -0.55, 0)}; 
    sk::vec3f t2[3] = {sk::vec3f(-0.55, -0.625, 0),
	           sk::vec3f(-0.7, -0.6, 0),
		   sk::vec3f(-0.675, -0.55, 0)};*/
    sk::vec3f t0[3] = { sk::vec3f(0.f, 0.5f, 0.f),
        sk::vec3f(0.f, 0.f, 0),
        sk::vec3f(-0.5f, -0.5f, 0) };
    sk::vec3f t1[3] = { sk::vec3f(0.f, 0.5f, 0.f),
        sk::vec3f(0.f, 0.f, 0),
        sk::vec3f(0.5f, -0.5f, 0) };
    sk::vec3f t2[3] = { sk::vec3f(0.f, 0.f, 0),
        sk::vec3f(0.5f, -0.5f, 0),
        sk::vec3f(0.25f, -0.35f, 0) };

    float cy[4] = { 0.5f, 0.25f, -0.25f, -0.5f };
    float cx[4] = { 0.25f, 0.5f, 0.5f, 0.25f };

    /*r.draw_filled_triangle(t0[0], t0[1], t0[2], green);
    r.draw_filled_triangle(t1[0], t1[1], t1[2], red);
    r.draw_filled_triangle(t2[0], t2[1], t2[2], white);*/

    /*auto v0 = sk::vec3f(0.25f, 0.5f, 0.f);
    auto v1 = sk::vec3f(0.f, 0.f, 0.f);
    auto v2 = sk::vec3f(0.25f, 0.5f, 0.f);

    for (size_t i = 0; i < 8; ++i)
    {
        v2 = v0;
        v0[0] = cx[i % 4] * (i < 4 ? (-1) : 1);
        v0[1] = cy[i % 4] * (i < 4 ? 1 : (-1));
        r.draw_filled_triangle(v0, v1, v2, (i % 2 == 0) ? green : red);
    }*/
    sk::vec3f light_dir(0, 0, -1);

    for (auto& face : m.faces) {
        auto& v0 = m.verts[face[0]];
        auto& v1 = m.verts[face[1]];
        auto& v2 = m.verts[face[2]];
        sk::vec3f n = cross((v2 - v0), (v1 - v0));
        n.norm();
        float intensity = dot(n, light_dir);
        if (intensity > 0)
        {
            r.draw_filled_triangle(v0, v1, v2, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
        else
        {
            r.draw_filled_triangle(v0, v1, v2, red);
        }
    }

    return 0;
}

