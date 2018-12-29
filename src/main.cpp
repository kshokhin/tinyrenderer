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
    model m("../models/african_head.obj");
    sk::renderer r(image);
    sk::vec3f t0[3] = {sk::vec3f(-0.975, -0.825, 0),
	           sk::vec3f(-0.875, -0.6, 0),
		   sk::vec3f(-0.825, -0.8, 0)}; 
    sk::vec3f t1[3] = {sk::vec3f(-0.55, -0.875, 0),
	           sk::vec3f(-0.625, -1, 0),
		   sk::vec3f(-0.825, -0.55, 0)}; 
    sk::vec3f t2[3] = {sk::vec3f(-0.55, -0.625, 0),
	           sk::vec3f(-0.7, -0.6, 0),
		   sk::vec3f(-0.675, -0.55, 0)};

    r.draw_filled_triangle(t0[0], t0[1], t0[2], green);
    r.draw_filled_triangle(t1[0], t1[1], t1[2], red);
    r.draw_filled_triangle(t2[0], t2[1], t2[2], white);
    return 0;
}

