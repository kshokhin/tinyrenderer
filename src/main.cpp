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
    r.draw(m);
    return 0;
}

