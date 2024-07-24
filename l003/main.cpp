#include <cmath>
#include <iostream>
#include <limits>

#include "geometry.h"
#include "model.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);
Model* model = NULL;
const int width = 800;
const int height = 800;

// point-point
void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
    bool isSteep = false;
    // when @anchor 1 facing p0.x == p1.x, the image will not draw properly,
    // because the loop only execute once
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        isSteep = true;
    }
    if (p0.x > p1.x) {
        std::swap(p0.x, p1.x);
        std::swap(p0.y, p1.y);
    }
    int cnt = 0;
    for (int x = p0.x; x <= p1.x; x++) {
        ///@anchor 1
        float t = (x - p0.x) / (float)(p1.x - p0.x);
        int y = p0.y * (1. - t) + p1.y * t;
        if (isSteep)
            image.set(y, x, color);
        else
            image.set(x, y, color);
    }
}

void rasterize(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color,
               int yBuffer[]) {
    if (p0.x > p1.x) std::swap(p0, p1);
    for (int x = p0.x; x <= p1.x; x++) {
        float t = (x - p0.x) / (float)(p1.x - p0.x);
        int y = p0.y * (1. - t) + p1.y * t + .5;
        if (yBuffer[x] < y) {
            yBuffer[x] = y;
            image.set(x, 0, color);
        }
    }
}

int main(int argc, char** argv) {
    {
        TGAImage scene(width, height, TGAImage::RGB);

        // scene "2d mesh"
        line(Vec2i(20, 34), Vec2i(744, 400), scene, red);
        line(Vec2i(120, 434), Vec2i(444, 400), scene, green);
        line(Vec2i(330, 463), Vec2i(594, 200), scene, blue);

        // screen line
        line(Vec2i(10, 10), Vec2i(790, 10), scene, white);
        // i want to have the origin at the left bottom corner of the image
        scene.flipVertically();
        scene.writeTGAFile("output.tga");
    }
    {
        TGAImage render(width, 16, TGAImage::RGB);
        int ybuffer[width];
        for (int i = 0; i < width; i++) {
            ybuffer[i] = std::numeric_limits<int>::min();
        }
        rasterize(Vec2i(20, 34), Vec2i(744, 400), render, red, ybuffer);
        // rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green, ybuffer);
        // rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue, ybuffer);

        // 1-pixel wide image is bad for eyes, lets widen it
        for (int i = 0; i < width; i++) {
            for (int j = 1; j < 16; j++) {
                render.set(i, j, render.get(i, 0));
            }
        }
        // i want to have the origin at the left bottom corner of the image
        render.flipVertically();

        render.writeTGAFile("render.tga");
    }
    return 0;
}
