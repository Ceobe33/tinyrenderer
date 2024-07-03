#include <iostream>

#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

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

int main(int argc, char** argv) {
    TGAImage image(200, 200, TGAImage::RGB);
    image.set(0, 0, red);
    for (int i = 0; i < 1000000; i++) {
        line(13, 20, 80, 40, image, white);
        line(20, 13, 40, 80, image, red);
        line(80, 40, 13, 20, image, red);
    }

    // i want to have the origin at the left bottom corner of the image
    image.flipVertically();
    image.writeTGAFile("output.tga");
    /** profiling link YouTube https://youtu.be/zbTtVW64R_I?si=W3LVYgRI8mxgfNAe
     * gprof website https://sourceware.org/binutils/docs/gprof/
     *  `$ g++ -O0 -pg main.cpp tgaimage.h tgaimage.cpp`
     *    to generate a.out file, then `$ ./a.out` to generate gmon.out
     *  install gprof2dot over https://github.com/jrfonseca/gprof2dot.git
     *  -w is show function name for involke
     *  `$ gprof a.out gmon.out | gprof2dot.py -w | dot -Tpng -o output.png`
     */
    return 0;
}
