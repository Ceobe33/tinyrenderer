#include <math.h>

#include <iostream>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        image.set(x, y, color);
    }
}

void line(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor color) {
    int x0 = t0.u, y0 = t0.v;
    int x1 = t1.u, y1 = t1.v;
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        image.set(x, y, color);
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
    // sort the vertices, lower to upper, bubble-sort
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    line(t0, t1, image, green);
    line(t1, t2, image, green);
    line(t2, t0, image, red);
}

int main(int argc, char** argv) {
    std::cout << std::endl << argc << std::endl;
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    triangle(Vec2i(10, 10), Vec2i(35, 25), Vec2i(30, 81), image, white);
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, white);
        }
    }

    // i want to have the origin at the left bottom corner of the image
    image.flipVertically();
    image.writeTGAFile("output.tga");
    delete model;
    /** profiling link YouTube https://youtu.be/zbTtVW64R_I?si=W3LVYgRI8mxgfNAe
     * gprof website https://sourceware.org/binutils/docs/gprof/
     *  `$ g++ -O0 -pg main.cpp tgaimage.h tgaimage.cpp`
     *    to generate a.out file, then `$ ./a.out` to generate gmon.out
     *  -w is show function name for involke
     *  install gprof2dot over https://github.com/jrfonseca/gprof2dot.git
     *  `$ gprof a.out gmon.out | gprof2dot.py -w | dot -Tpng -o output.png`
     */
    return 0;
}
