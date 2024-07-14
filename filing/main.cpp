#include <iostream>

#include "../l001/tgaimage.h"
const TGAColor red = TGAColor(255, 0, 0, 255);

int main(int argc, char *argv[]) {
    int x = 0;
    TGAImage image(200, 200, TGAImage::RGB);
    while (x++ < 100000000) {
        int y = 100;
        while (y-- > 0) {
            image.set(0, 0, red);
        }
    }
    return 0;
}
