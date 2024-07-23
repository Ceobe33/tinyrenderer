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
    bool isSteep = false;
    // when @anchor 1 facing x0 == x1, the image will not draw properly, because
    // the loop only execute once
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        isSteep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int cnt = 0;
    for (int x = x0; x <= x1; x++) {
        ///@anchor 1
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        if (isSteep)
            image.set(y, x, color);
        else
            image.set(x, y, color);
    }
}

void line(Vec2i t0, Vec2i t1, TGAImage& image, TGAColor color) {
    int x0 = t0.u, y0 = t0.v;
    int x1 = t1.u, y1 = t1.v;
    line(x0, y0, x1, y1, image, color);
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
    triangle(Vec2i(300, 710), Vec2i(350, 25), Vec2i(100, 100), image, white);
    triangle(Vec2i(500, 710), Vec2i(550, 225), Vec2i(700, 200), image, white);

    line(100, 100, 300, 0, image, white);
    line(100, 100, 400, 100, image, white);
    line(100, 100, 400, 400, image, white);
    line(100, 700, 100, 100, image, white);
    line(0, 600, 100, 100, image, white);
    line(0, 200, 100, 100, image, white);
    line(0, 100, 100, 100, image, white);
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
