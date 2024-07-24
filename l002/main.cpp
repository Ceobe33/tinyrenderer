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

// point-point
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
    int x0 = t0.x, y0 = t0.y;
    int x1 = t1.x, y1 = t1.y;
    line(x0, y0, x1, y1, image, color);
}

// point-line
/*
void line(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image,
          TGAColor color = white) {
    int x0 = t1.x, y0 = t1.y;
    int x1 = t2.x, y1 = t2.y;
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
            line(t0, Vec2i(y, x), image, color);
        else
            line(t0, Vec2i(x, y), image, color);
    }
}
*/

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image,
              TGAColor color = white) {
    // sort the vertices, lower to upper, bubble-sort
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int tolHeight = t2.y - t0.y;

    for (int y = t0.y; y <= t2.y; y++) {
        bool isL = y <= t1.y;
        int segHeight = (isL ? t1.y - t0.y : t2.y - t1.y) + 1;
        float alpha = (float)(y - t0.y) / tolHeight;
        float beta = (float)(isL ? (y - t0.y) : (y - t1.y)) / segHeight;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = (isL ? (t0 + (t1 - t0) * beta) : (t1 + (t2 - t1) * beta));
        image.set(A.x, y, color);
        image.set(B.x, y, color);
        if (A.x < B.x) std::swap(A, B);
        for (int x = B.x + 1; x < A.x; x++) {
            image.set(x, y, color);
        }
    }
    // line(t2, t0, t1, image, white);
    // line(t0, t1, image, green);
    // line(t1, t2, image, green);
    // line(t2, t0, image, red);
}

int main(int argc, char** argv) {
    std::cout << std::endl << argc << std::endl;
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    // triangle(Vec2i(300, 710), Vec2i(350, 025), Vec2i(100, 100), image);
    // triangle(Vec2i(500, 710), Vec2i(550, 225), Vec2i(700, 200), image);
    // triangle(Vec2i(100, 800), Vec2i(200, 600), Vec2i(100, 300), image);

    Vec3f lightDir(0, 0, -1);
    /** random color fill
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screenCoords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f worldCoords = model->vert(face[j]);
            screenCoords[j] = Vec2i((worldCoords.x + 1.) * width / 2.,
                                    (worldCoords.y + 1.) * height / 2.);
        }
        triangle(screenCoords[0], screenCoords[1], screenCoords[2], image,
                 TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
    }
    */
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] =
                Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^
                  (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * lightDir;
        if (intensity > 0) {
            triangle(screen_coords[0], screen_coords[1], screen_coords[2],
                     image,
                     TGAColor(intensity * 255, intensity * 255, intensity * 255,
                              255));
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
