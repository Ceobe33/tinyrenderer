#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tgaimage.h"

TGAImage::TGAImage() : data(NULL), width(0), height(0), byteSpp(0) {}

TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), byteSpp(bpp) {
    unsigned long nBytes = width * height * byteSpp;
    data = new unsigned char[nBytes];
    memset(data, 0, nBytes);
}

