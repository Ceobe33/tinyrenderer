#include <cmath>

// #include "../cppModules/profiler.cpp"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    for (float t = 0.; t < 1.; t += .01) {
        int x = x0 + (x1 - x0) * t;
        int y = y0 + (y1 - y0) * t;
        image.set(x, y, color);
    }
}

int main(int argc, char** argv) {
    // Instrumentor::Get().LaunchSession("myprofiler", "profiler.json");

    TGAImage image(200, 200, TGAImage::RGB);
    image.set(0, 0, red);
    line(0, 0, 40, 20, image, white);
    line(10, 10, 200, 100, image, white);
    line(20, 13, 40, 80, image, red);
    line(80, 40, 13, 20, image, red);

    // i want to have the origin at the left bottom corner of the image
    image.flipVertically();
    image.writeTGAFile("output.tga");
    // Instrumentor::Get().EndSession();

    /** profiling link YouTube https://youtu.be/zbTtVW64R_I?si=W3LVYgRI8mxgfNAe
     * gprof website https://sourceware.org/binutils/docs/gprof/
     *  `$ g++ -O0 -pg main.cpp tgaimage.h tgaimage.cpp`
     *    to generate a.out file, then `$ ./a.out` to generate gmon.out
     *  -w is show function name for involke
     *  install gprof2dot over https://github.com/jrfonseca/gprof2dot.git
     *  `$ gprof a.out gmon.out | gprof2dot.py -w | dot -Tpng -o output.png`
     *
     * g++ -pg -g -ggdb -O0 main.cpp tgaimage.h tgaimage.cpp gprof_helper.so
     *
     * Error
     * `$ cc -o main.cpp tgaimage.h tgaimage.cpp -g -pg`
     * tgaimage.h:4:10: fatal error: fstream: No such file or directory
     *     4 | #include <fstream>

     *
     *  generate threads
     * helper(https://blog.csdn.net/stanjiang2010/article/details/5655143)
     *  `$ gcc -shared -fPIC gprof_helper.c -o gprof_helper.so -lpthread -ldl`
     */

    /** 选取头几行输出

        aaron@vyunfeiyi-PC0:~/tinyrenderer/tinyrenderer/l001$ gprof a.out
       gmon.out 2>&1 | head -n 10 Flat profile:

        Each sample counts as 0.01 seconds.
         no time accumulated

          %   cumulative   self              self     total
         time   seconds   seconds    calls  Ts/call  Ts/call  name
          0.00      0.00     0.00 160000001     0.00     0.00
        TGAColor::TGAColor(TGAColor const&) 0.00      0.00     0.00 157000001
       0.00 0.00  TGAImage::set(int, int, TGAColor) 0.00      0.00     0.00
       6000000 0.00 0.00  std::remove_reference<int&>::type&&
       std::move<int&>(int&)

     */
    return 0;
}
